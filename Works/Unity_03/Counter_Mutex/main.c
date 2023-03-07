// Author: Helder Henrique da Silva
// Year: 2022/2
//
// Description: Programa que cria 128 threads secundárias que incrementam uma variável global 100 vezes cada uma.
//              O programa utiliza mutex para eliminar a condição de corrida.
//

#include <stdio.h>
#include <pthread.h>

// Definindo a quantidade de threads
#define N 128

// Variável global
int x = 0;

// Mutex
pthread_mutex_t mutex;

// Função executada pelas threads secundárias
// Incrementa a variável global 100 vezes por thread eliminando a condição de corrida com mutex
void *countThread(void *arg)
{
  int i = 0;
  while (i < 100)
  {
    pthread_mutex_lock(&mutex);
    x++;
    pthread_mutex_unlock(&mutex);
    i++;
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv)
{
  // Cria vetor do tipo thread e inicializa mutex
  pthread_t threads[N];
  pthread_mutex_init(&mutex, NULL);

  // Cria as threads secundárias passando a função countThread como parâmetro e sincroniza as threads
  for (int counterThread = 0; counterThread < N; counterThread++)
  {
    pthread_create(&threads[counterThread], NULL, countThread, NULL);
    pthread_join(threads[counterThread], NULL);
  }

  // Libera o mutex
  pthread_mutex_destroy(&mutex);

  // Imprime o valor final da variável global
  printf("x = %d\n", x);
  return 0;
}