// Author: Helder Henrique da Silva
// Year: 2022/2
//
// Description: Implementação de um programa que verifica se um número é primo ou não.
//

#include <stdio.h>
#include <pthread.h>

// Verificar por meio de threads se o numero é primo
// Usar 2 threads para verificar os fatores do numero n informado
// Se n for primo, imprimir 1
// Se n não for primo, imprimir 0

// Variaveis globais
int fator1 = 0;
int fator2 = 0;

// Threads
pthread_t t1, t2;
pthread_mutex_t mutex;

// Função para a primeira thread
// Verifica se x é divisível pelos valores no intervalo [2, x/2-1]
void *verificaMetade(void *arg)
{
    // Lock no mutex
    pthread_mutex_lock(&mutex);
    int x = *(int *)arg;
    for (int i = 2; i < x / 2; i++)
    {
        // Se for divísiel, então não é primo
        // Cancela a thread2 e termina a execução da thread1
        if (x % i == 0)
        {
            pthread_cancel(t2);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }
    // Se não for divisível, então é primo
    // Muda o fator da primeira thread para 1 e termina a execução
    fator1 = 1;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);

    return NULL;
}

// Função para a segunda thread
// Verifica se x é divisível pelos valores no intervalo [x/2, x-1]
void *verificaResto(void *arg)
{
    // Lock no mutex
    pthread_mutex_lock(&mutex);
    int x = *(int *)arg;
    for (int i = (x / 2) + 1; i < x; i++)
    {
        // Se for divísiel, então não é primo
        // Cancela a thread1 e termina a execução da thread2
        if (x % i == 0)
        {
            pthread_cancel(t1);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
    }
    // Se não for divisível, então é primo
    // Muda o fator da segunda thread para 1 e termina a execução
    fator2 = 1;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);

    return NULL;
}

int main()
{
    int x;
    scanf("%d", &x);

    // Input > 1
    while (x <= 1)
    {
        printf("Numero invalido. Informe um numero maior que 1.\n");
        scanf("%d", &x);
    }

    // Inicializa o mutex
    pthread_mutex_init(&mutex, NULL);

    // Iniciar as threads passando o endereço de x como argumento
    pthread_create(&t1, NULL, verificaMetade, &x);
    pthread_create(&t2, NULL, verificaResto, &x);

    // Aguardar o término das threads e as sincroniza
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroi o mutex
    pthread_mutex_destroy(&mutex);

    // Verifica os fatores e imprime o resultado
    if (fator1 == 1 && fator2 == 1)
    {
        printf("1");
        return 1;
    }
    else
    {
        printf("0");
    }
    return 0;
}