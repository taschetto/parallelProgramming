#include "slave.h"
#include "tags.h"
#include <omp.h>
#include <stdlib.h>

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

Slave::Slave(int rank, int job_size, int thread_count, int sort_method)
  : rank(rank), job_size(job_size), thread_count(thread_count), sort_method(sort_method) {}

Slave::~Slave() {}

void Slave::mainLoop()
{
  MPI_Status status;

  // aloca memória para um buffer do tamanho de um job
  int* buffer = 0;
  int job_num = -1;
  bool must_die = false;

  omp_set_num_threads(this->thread_count);
  #pragma omp parallel private(buffer, job_num) firstprivate(must_die)
  {
    while (!must_die)
    {
      buffer = new int[this->job_size]();
      askForJob();
      job_num = receiveJob(buffer, &status);
      if (status.MPI_TAG != TAG_SUICIDE) {
        doJob(buffer);
        sendResultsToMaster(buffer, job_num);
      }
      else
        must_die = true;
    }
  }

  // libera a memória alocada para o buffer
  delete[] buffer;
}

void Slave::askForJob(void)
{
  #pragma omp critical(mpi)
  {
    MPI_Send(0, 0, MPI_INT, 0, TAG_JOB_NEEDED, MPI_COMM_WORLD);
  }
}

int Slave::receiveJob(int* buffer, MPI_Status* status)
{
  #pragma omp critical(mpi)
  {
    MPI_Recv(buffer, this->job_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, status);
  }
  return status->MPI_TAG;
}

void Slave::bubblesort(int* buffer)
{
  //printf("%d vai conquistar %lu números\n", rank, numbers.size());
  // método fornecido pelo professor para o bubblesort
  int c = 0, troca, trocou = 1;
  while (c < (this->job_size-1) & trocou)
  {
    trocou = 0;
    for (int d = 0 ; d < this->job_size - c - 1; d++)
    {
      if (buffer[d] > buffer[d + 1])
      {
        troca        = buffer[d];
        buffer[d]   = buffer[d+1];
        buffer[d + 1] = troca;
        trocou = 1;
      }
    }
    c++;
  }
}

void Slave::doJob(int* buffer)
{
  if(sort_method == 1) 
  {
    // realiza o quicksort no conteúdo do buffer
    qsort(buffer, this->job_size, sizeof(int), cmpfunc);
  }
  else 
  {
    bubblesort(buffer);
  }
}

void Slave::sendResultsToMaster(int* buffer, int job_num)
{
  #pragma omp critical(mpi)
  {
    MPI_Send(buffer, this->job_size, MPI_INT, 0, job_num, MPI_COMM_WORLD);
  }
}
