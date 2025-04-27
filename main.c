#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PENSANDO 0
#define FOME 1
#define COMENDO 2
#define FOI_EMBORA 3

int NUM_FILO = 5;
int MAX_COMIDA = 10;
int MAX_SLEEP = 1000000;

/*inicializa as estruturas*/
pthread_mutex_t* garfos;
pthread_mutex_t estado_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* conds;
pthread_mutex_t comida_mutex = PTHREAD_MUTEX_INITIALIZER;
int* estado_filosofo;
int* vezes_comeu;
int comida_restante;

/*visualização*/
const char* estado_para_string(int estado) {
    switch (estado) {
        case PENSANDO: return "🤔";
        case FOME: return "😐";
        case COMENDO: return "🍴";
        case FOI_EMBORA: return "💤";
        default: return "🤔";
    }
}

/*imprime as ações dos filósofos*/
void printar_estado_geral() {
    printf("\n[Comida restante: %d]\n", comida_restante);
    for (int i = 0; i < NUM_FILO; i++) {
        printf("Filósofo %d: %s  ", i, estado_para_string(estado_filosofo[i]));
    }
    printf("\n");
}

/*ação de pensar*/
void pensar(int id) {
    pthread_mutex_lock(&estado_mutex);
    estado_filosofo[id] = PENSANDO;
    printar_estado_geral();
    pthread_mutex_unlock(&estado_mutex);

    usleep(rand() % MAX_SLEEP);
}

/*chama os vizinhos previnindo starvation*/
void chamar_vizinho(int id) {
    if (estado_filosofo[id] == FOME &&
        estado_filosofo[ (id + NUM_FILO - 1) % NUM_FILO] != COMENDO &&
        estado_filosofo[(id + 1) % NUM_FILO] != COMENDO) {

        estado_filosofo[id] = COMENDO;
        pthread_cond_signal(&conds[id]);
    }
}

/*seleciona os garfos e o pega*/
void pegar_garfos(int id) {
    pthread_mutex_lock(&estado_mutex);

    estado_filosofo[id] = FOME;
    printar_estado_geral();

    chamar_vizinho(id);

    while (estado_filosofo[id] != COMENDO) {
        pthread_cond_wait(&conds[id], &estado_mutex);
    }

    pthread_mutex_unlock(&estado_mutex);

    int garfo_esq = id;
    int garfo_dir = (id + 1) % NUM_FILO;

    if (garfo_esq < garfo_dir) {
        pthread_mutex_lock(&garfos[garfo_esq]);
        pthread_mutex_lock(&garfos[garfo_dir]);
    } else {
        pthread_mutex_lock(&garfos[garfo_dir]);
        pthread_mutex_lock(&garfos[garfo_esq]);
    }
}

/*termina de comer e libera os garfos*/
void largar_garfos(int id) {
    int garfo_esq = id;
    int garfo_dir = (id + 1) % NUM_FILO;

    pthread_mutex_unlock(&garfos[garfo_esq]);
    pthread_mutex_unlock(&garfos[garfo_dir]);

    pthread_mutex_lock(&estado_mutex);

    estado_filosofo[id] = PENSANDO;
    printar_estado_geral();
    chamar_vizinho( (id + NUM_FILO - 1) % NUM_FILO);
    chamar_vizinho((id + 1) % NUM_FILO);

    pthread_mutex_unlock(&estado_mutex);
}

/*ação de comer*/
void comer(int id) {
    usleep(rand() % MAX_SLEEP);
}

/*loop de ações de um filosofo*/
void* filosofo(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while (1) {
        pensar(id);

        pegar_garfos(id);

        pthread_mutex_lock(&comida_mutex);
        if (comida_restante > 0) {
            comida_restante--;
            vezes_comeu[id]++;
            pthread_mutex_unlock(&comida_mutex);

            printar_estado_geral();
            comer(id);
            largar_garfos(id);
        } else {
            pthread_mutex_unlock(&comida_mutex);

            largar_garfos(id);
            pthread_mutex_lock(&estado_mutex);
            estado_filosofo[id] = FOI_EMBORA;
            printar_estado_geral();
            pthread_mutex_unlock(&estado_mutex);
            break;
        }
    }

    return NULL;
}

/*alocamos memoria e executamos até a comida acabar*/
int main(int argc, char* argv[]) {
    if (argc >= 2) NUM_FILO = atoi(argv[1]);
    if (argc >= 3) MAX_COMIDA = atoi(argv[2]);
    if (argc >= 4) MAX_SLEEP = atoi(argv[3]);

    if (NUM_FILO <= 1 || MAX_COMIDA <= 0 || MAX_SLEEP <= 0) {
        printf("Entrada incorreta.\nInsira: ./main [num_filósofos] [porções_comida] [delay]\n");
        exit(1);
    }

    srand(time(NULL));

    garfos = malloc(NUM_FILO * sizeof(pthread_mutex_t));
    estado_filosofo = malloc(NUM_FILO * sizeof(int));
    vezes_comeu = malloc(NUM_FILO * sizeof(int));
    conds = malloc(NUM_FILO * sizeof(pthread_cond_t));
    comida_restante = MAX_COMIDA;

    pthread_t threads[NUM_FILO];

    for (int i = 0; i < NUM_FILO; i++) {
        pthread_mutex_init(&garfos[i], NULL);
        pthread_cond_init(&conds[i], NULL);
        estado_filosofo[i] = PENSANDO;
        vezes_comeu[i] = 0;
    }

    for (int i = 0; i < NUM_FILO; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, filosofo, id);
    }

    for (int i = 0; i < NUM_FILO; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nTodos os filósofos terminaram.\nResultado final:\n");
    for (int i = 0; i < NUM_FILO; i++) {
        printf("%d: comeu %d vezes 🍽️.\n", i, vezes_comeu[i]);
    }

    free(garfos);
    free(estado_filosofo);
    free(vezes_comeu);
    free(conds);

    return 0;
}
