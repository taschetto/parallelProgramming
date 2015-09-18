#include "slave.h"
#include "tags.h"
#include "mpi.h"
#include <stdlib.h>

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

Slave::Slave(int job_size)
  : job_size(job_size)
{
}

Slave::~Slave()
{
}

void Slave::mainLoop()
{
  int* data = new int[this->job_size]();
  MPI_Status status;

  while (true)
  {
    // Avisa o master que está disponível
    MPI_Send(data, this->job_size, MPI_INT, 0, TAG_IM_FREE, MPI_COMM_WORLD);
    // Recebe uma mensagem
    MPI_Recv(data, this->job_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // Se é uma ordem para suicídio, quebra o loop
    if (status.MPI_TAG == TAG_SUICIDE) break;
    // Se não, realiza o quicksort
    qsort(data, this->job_size, sizeof(int), cmpfunc);
    // E envia o resultado de volta pro master
    MPI_Send(data, this->job_size, MPI_INT, 0, TAG_JOB_DONE, MPI_COMM_WORLD);
  }

  delete[] data;
}