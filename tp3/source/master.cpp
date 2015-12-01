#include "master.h"
#include "tags.h"
#include <stdio.h>
#include <cstring>

Master::Master(int rank, int num_slaves, int num_jobs, int job_size, int thread_count)
  : rank(rank),
    num_slaves(num_slaves),
    num_jobs(num_jobs),
    job_size(job_size),
    thread_count(thread_count),
    next_job(0),
    jobs_done(0),
    dead_slaves(0)
{
  // alocação dinâmica dos jobs
  this->jobs = new int*[this->num_jobs];
  for (int i = 0; i < this->num_jobs; i++)
  {
    this->jobs[i] = new int[this->job_size];
    for (int j = 0; j < this->job_size; j++)
    {
      this->jobs[i][j] = (i + 1) * (this->job_size - j);
    }
  }
}

Master::~Master()
{
  // libera a memória alocada para os jobs e estrutura auxiliar
  for (int i = 0; i < this->num_jobs; i++)
    delete[] this->jobs[i];
  delete[] this->jobs;
}

void Master::printJobs()
{
  /* Imprime a matriz */
  for (int i = 0; i < this->num_jobs; i++)
  {
    printf("Job %2d:\t", i);
    for (int j = 0; j < this->job_size; j++)
      printf("%3d ", this->jobs[i][j]);
    printf("\n");
  }
}

void Master::mainLoop()
{
  MPI_Status status;
  int* buffer = new int[job_size]();

  while (hasWaitingJobs() or hasWorkingJobs() or hasSlavesAlive())
  {
    receive(buffer, &status);

    if (status.MPI_TAG == TAG_JOB_NEEDED)
    {
      if (hasWaitingJobs())
        sendJobToSlave(status.MPI_SOURCE);
      else
        killSlave(status.MPI_SOURCE);
    }
    else
      getResultsFromSlave(status.MPI_TAG, buffer);
  }

  // libera a memória alocada para o buffer
  delete[] buffer;
}

void Master::receive(int* buffer, MPI_Status* status)
{
  // Recebe uma mensagem e salva no buffer
  MPI_Recv(buffer, job_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

void Master::sendJobToSlave(int slave_pid)
{
  // envia um novo job para este escravo e associa o escravo ao job enviado
  MPI_Send(this->jobs[this->next_job], this->job_size, MPI_INT, slave_pid, this->next_job, MPI_COMM_WORLD);
  this->next_job++;
}

void Master::getResultsFromSlave(int tag, int* buffer)
{
  // atualiza o job original com os dados do resultado e contabiliza a conclusão
  memcpy(this->jobs[tag], buffer, sizeof(int) * this->job_size);
  this->jobs_done++;
}

void Master::killSlave(int slave_pid)
{
  // embora o escravo esteja disponível, não há mais jobs - logo, ele deve MORRER :D
  MPI_Send(0, 0, MPI_INT, slave_pid, TAG_SUICIDE, MPI_COMM_WORLD);
  this->dead_slaves++;
}
