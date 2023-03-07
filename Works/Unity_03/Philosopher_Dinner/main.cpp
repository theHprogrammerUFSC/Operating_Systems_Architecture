// Author: Helder Henrique da Silva
// Year: 2022/2
//
// Description: Implementação de um programa que simula o jantar dos filósofos.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <vector>
#include <condition_variable>

using namespace std;

// Variáveis globais, mutex e condição
// Vetor de estados dos filósofos (Pensando, Faminto, Comendo)
mutex mtx;
condition_variable cv;
vector<char> estadosDosFilosofos = {'P', 'P', 'P', 'P', 'P'};
int n = 5;

// Função para printar os estados
void printarEstados()
{
    for (int i = 0; i < n; i++)
    {
        cout << estadosDosFilosofos[i];
    }
    cout << endl;
}

// Função para pegar o estado do filósofo
char pegarEstado(int i)
{
    return estadosDosFilosofos[i];
}

// Função para pegar o estado do filósofo da esquerda
// Para retornar o estado do filósofo da esquerda, pega o resto da divisão do (numero do filósofo + 1) por 5
char pegarEstadoEsquerda(int i)
{
    return estadosDosFilosofos[(i + 1) % n];
}

// Função para pegar o estado do filósofo da direita
// Para retornar o estado do filoso da direita, pega o resto da divisão do (número do filósofo + 4) por 5
char pegarEstadoDireita(int i)
{
    return estadosDosFilosofos[(i + n - 1) % n];
}

// Função para dormir a thread antes de passar para o proximo estado
void dormir()
{
    this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
}

// O filosofo começa pensando, depois fica faminto e pega os garfos, depois come e depois solta os garfos
// O filosofo só pode comer se os dois garfos vizinhos estiverem livres
void StartJantarFilosofico(int i)
{
    while (true)
    {
        // Pensando
        // Usa o unique_lock para garantir que o mutex seja desbloqueado quando a thread for dormir
        // Estado do filosofo é alterado para pensando
        // Printa os estados
        // Dorme a thread
        {
            unique_lock<mutex> lck(mtx);
            estadosDosFilosofos[i] = 'P';
            printarEstados();
        }
        dormir();

        // Faminto
        // Usa o unique_lock para garantir que o mutex seja desbloqueado quando a thread for dormir
        // Estado do filosofo é alterado para faminto
        // Printa os estados
        // Dorme a thread
        {
            unique_lock<mutex> lck(mtx);
            estadosDosFilosofos[i] = 'F';
            printarEstados();
        }
        dormir();

        // Comendo
        // Usa o unique_lock para garantir que o mutex seja desbloqueado quando a thread for dormir
        // Enquanto o filosofo da esquerda ou da direita estiver comendo, a thread fica bloqueada usando a condição cv.wait()
        // Estado do filosofo é alterado para comendo
        // Printa os estados
        // Dorme a thread
        {
            unique_lock<mutex> lck(mtx);
            while (pegarEstadoEsquerda(i) == 'C' || pegarEstadoDireita(i) == 'C')
            {
                cv.wait(lck);
            }
            estadosDosFilosofos[i] = 'C';
            printarEstados();
        }
        dormir();

        // Solta os garfos
        // Usa o unique_lock para garantir que o mutex seja desbloqueado quando a thread for dormir
        // Estado do filosofo é alterado para pensando
        // Printa os estados
        // Usa a condição cv.notify_all() para acordar todas as threads bloqueadas
        // Dorme a thread
        {
            unique_lock<mutex> lck(mtx);
            estadosDosFilosofos[i] = 'P';
            printarEstados();
            cv.notify_all();
        }
        dormir();
    }
}

int main()
{
    // Cria as threads
    thread filosofos[5];

    // Inicializa as threads chamando a função filosofo para eles começarem seu jantar
    for (int i = 0; i < n; i++)
    {
        filosofos[i] = thread(StartJantarFilosofico, i);
    }

    // Espera as threads terminarem de jantar e as sincroniza
    for (int i = 0; i < n; i++)
    {
        filosofos[i].join();
    }

    return 0;
}