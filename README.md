# Jantar dos filósofos - Projeto Multhithread

## Objetivo

Este projeto implementa uma aplicação multithread inspirada no clássico problema dos filósofos comensais (jantar dos filósofos), utilizando **mutex locks** e **variáveis de condição** para garantir a sincronização entre threads. A aplicação também fornece uma visualização em terminal do progresso da simulação.

## Descrição Geral

Ações principais de um filósofo

- Pensar (pensar()): O filósofo "pensa" por um tempo aleatório.

- Ficar com fome (pegar_garfos()): Ele tenta pegar os dois garfos.

- Comer (comer()): Se conseguir os garfos, ele come uma porção (se ainda houver).

- Largar os garfos (largar_garfos()): Libera os garfos e possibilita que os vizinhos comam.

- Sair (foi_embora()): Quando a comida acaba, o filósofo "vai embora".


## Estrutura de Sincronização Utilizada
- **Mutexes: `garfos[i]`**: para evitar que dois filósofos usem o mesmo garfo simultaneamente.
- **Mutex: `estado_mutex  e comida_mutex`**: protege a leitura e escrita dos estados dos filósofos e da comida restante.
- **Condição: `conds`**: avisa que um garfo foi liberado para os filósofos famintos.

## Visualização

A cada mudança de estado (pensando, com fome, comendo ou indo embora), a simulação imprime um painel no terminal indicando:

- Comida restante

- Estado de cada filósofo com emojis:
  - 🤔 Pensando
  - 😐 Com fome
  - 🍴 Comendo
  - 💤 Foi embora (com fome)

- No final, quanto cada filósofo comeu 

## Parâmetros que podem ser modificados

- num_filósofos: número de filósofos (mínimo 2)

- porções_comida: quantidade total de porções disponíveis na mesa

- delay: tempo máximo de espera (em microssegundos) para ações de pensar/comer

## Compilação

Para compilar o programa, utilize:

```bash
gcc -o main main.c -lpthread
./main [num_filósofos] [porções_comida] [delay]