#include "BancoDeDados.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Valores que parecem faltar dentro do private da classe BancoDeDados
// variável para a contagem dos escritores
int contagemEscritores{0};

// Mutexes para os leitores e escritores
std::mutex m_leitores;
std::mutex m_escritores;

// Variáveis de condição para os leitores e escritores
std::condition_variable cv_leitores;
std::condition_variable cv_escritores;

// Usa unique_lock para gerenciar o bloqueio e desbloqueio do mutex
// Enquanto a contagem dos escritores for maior que zero, o leitor deve esperar
// Incrementa a contagem de leitores e libera o mutex
// Le o valor da variavel
// Decrementa a contagem de leitores e notifica os escritores
// Retorna o valor lido
int BancoDeDados::read()
{
  std::unique_lock<std::mutex> lock(m_leitores);
  while (contagemEscritores > 0)
  {
    cv_leitores.wait(lock);
  }
  contagemLeitores++;
  lock.unlock();
  int valor = var;
  lock.lock();
  contagemLeitores--;
  if (contagemLeitores == 0)
  {
    cv_escritores.notify_one();
  }
  return valor;
}

// Usa unique_lock para gerenciar o bloqueio e desbloqueio do mutex
// Enquanto a contagem dos leitores for maior que zero ou a contagem dos escritores for maior que zero, o escritor deve esperar
// Incrementa a contagem de escritores e libera o mutex
// Incrementa o valor da variavel
// Decrementa a contagem de escritores e notifica os leitores e escritores
// Retorna o novo valor da variavel
int BancoDeDados::increment()
{
  std::unique_lock<std::mutex> lock(m_escritores);
  while (contagemLeitores > 0 || contagemEscritores > 0)
  {
    cv_escritores.wait(lock);
  }
  contagemEscritores++;
  lock.unlock();
  var++;
  lock.lock();
  contagemEscritores--;
  cv_escritores.notify_one();
  cv_leitores.notify_all();
  return var;
}