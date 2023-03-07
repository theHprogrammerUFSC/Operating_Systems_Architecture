#ifndef MYSEMAPHORE
#define MYSEMAPHORE

#include "AbstractSemaphore.hpp"

class MySemaphore : public AbstractSemaphore
{
private:
  std::mutex semaphoreMutex;
  std::condition_variable semaphoreCV;

public:
  // Construtor
  MySemaphore(int count = 1) : AbstractSemaphore(count) {}

  /**
   * @brief Libera semaforo
   */
  void release() override
  {
    // Usa o unique_lock para gerenciar o mutex que sera bloqueado e desbloqueado
    // Após o bloqueio do mutex, incrementa o contador de threads que estao usando o semaforo
    // Após o incremento, notifica uma thread que esta esperando o semaforo
    // Após a notificação, o mutex é desbloqueado
    std::unique_lock<std::mutex> lock(semaphoreMutex);
    m_nfree++;
    semaphoreCV.notify_one();
  }

  /**
   * @brief Obtem semaforo
   */
  void acquire() override
  {
    // Usa o unique_lock para gerenciar o mutex que sera bloqueado e desbloqueado
    // Após o bloqueio do mutex, decrementa o contador de threads que estao usando o semaforo
    // Caso o contador seja menor ou igual a zero, a thread é bloqueada até que outra thread libere o semaforo
    // Após a liberação do semaforo, o mutex é desbloqueado
    std::unique_lock<std::mutex> lock(semaphoreMutex);
    while (m_nfree <= 0)
    {
      semaphoreCV.wait(lock);
    }
    m_nfree--;
  }
};

#endif