#include "slave.h"
#include "tags.h"
#include "mpi.h"
#include <stdlib.h>

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

Slave::Slave(int job_size)
  : job_size(job_size) {}

Slave::~Slave() {}

void Slave::mainLoop()
{
  MPI_Status status;

  // aloca memória para um buffer do tamanho de um job
  int* buffer = new int[this->job_size]();

  while (true) // repete até que o mestre mande se suicidar
  {
    // Avisa o master que está disponível
    MPI_Send(buffer, this->job_size, MPI_INT, 0, TAG_IM_FREE, MPI_COMM_WORLD);
    // Recebe uma mensagem qualquer
    MPI_Recv(buffer, this->job_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // Se é uma ordem para suicídio, quebra o loop
    if (status.MPI_TAG == TAG_SUICIDE) break;
    // Se não, realiza o quicksort no conteúdo do buffer
    qsort(buffer, this->job_size, sizeof(int), cmpfunc);
    // E envia o resultado de volta pro master
    MPI_Send(buffer, this->job_size, MPI_INT, 0, TAG_JOB_DONE, MPI_COMM_WORLD);
  }

  // libera a memória alocada para o buffer
  delete[] buffer;
}