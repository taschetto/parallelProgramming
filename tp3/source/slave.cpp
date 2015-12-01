#include "slave.h"
#include "tags.h"
#include <stdlib.h>

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

Slave::Slave(int rank, int job_size)
  : rank(rank), job_size(job_size) {}

Slave::~Slave() {}

void Slave::mainLoop()
{
  MPI_Status status;

  // aloca memória para um buffer do tamanho de um job
  int* buffer = new int[this->job_size]();
  int job_num = -1;

  while (true) // repete até que o mestre mande se suicidar
  {
    askForJob();
    job_num = receiveJob(buffer, &status);
    printf("Slave %d received tag %d.\n", this->rank, status.MPI_TAG);
    if (status.MPI_TAG == TAG_SUICIDE) break; // Se é uma ordem para suicídio, quebra o loop
    doJob(buffer);
    sendResultsToMaster(buffer, job_num);
  }

  // libera a memória alocada para o buffer
  delete[] buffer;
}

void Slave::askForJob(void)
{
  MPI_Send(0, 0, MPI_INT, 0, TAG_JOB_NEEDED, MPI_COMM_WORLD);
}

int Slave::receiveJob(int* buffer, MPI_Status* status)
{
  // Recebe uma mensagem qualquer
  MPI_Recv(buffer, this->job_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status);
  return status->MPI_TAG;
}

void Slave::doJob(int* buffer)
{
  // realiza o quicksort no conteúdo do buffer
  qsort(buffer, this->job_size, sizeof(int), cmpfunc);
}

void Slave::sendResultsToMaster(int* buffer, int job_num)
{
  // E envia o resultado de volta pro master
  MPI_Send(buffer, this->job_size, MPI_INT, 0, job_num, MPI_COMM_WORLD);
}
