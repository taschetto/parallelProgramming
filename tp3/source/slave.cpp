#include "slave.h"
#include "tags.h"
#include <omp.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

Slave::Slave(int rank, int thread_count, int job_size)
  : rank(rank), thread_count(thread_count), job_size(job_size), must_die(false) {
    omp_set_num_threads(thread_count);
  }

Slave::~Slave() {}

bool Slave::mustRun() {
  #pragma omp critical
  {
    bool must_die = this->must_die;
  }
  return !must_die;
}

void Slave::mainLoop()
{
  #pragma omp parallel
  {
    MPI_Status status;

    // aloca memória para um buffer do tamanho de um job
    int* buffer = new int[this->job_size]();

    while (this->mustRun()) // repete até que o mestre mande se suicidar
    {
      receiveJob(buffer, &status);
      if (status.MPI_TAG != TAG_SUICIDE)
      {
        doJob(buffer);
        sendResultsToMaster(buffer);
      }
      else
      {
        #pragma omp critical
        must_die = true;
      }
    }

    // libera a memória alocada para o buffer
    delete[] buffer;
  }
}

void Slave::receiveJob(int* buffer, MPI_Status* status)
{
  // Recebe uma mensagem qualquer
  MPI_Recv(buffer, this->job_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

void Slave::doJob(int* buffer)
{
  // realiza o quicksort no conteúdo do buffer
  qsort(buffer, this->job_size, sizeof(int), cmpfunc);
}

void Slave::sendResultsToMaster(int* buffer)
{
    // E envia o resultado de volta pro master
    MPI_Send(buffer, this->job_size, MPI_INT, 0, TAG_JOB_DONE, MPI_COMM_WORLD);
}
