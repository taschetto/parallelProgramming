#include "master.h"
#include "tags.h"
#include "mpi.h"
#include <stdio.h>
#include <cstring>

Master::Master(int num_slaves, int num_jobs, int job_size)
  : num_slaves(num_slaves),
    num_jobs(num_jobs),
    job_size(job_size),
    next_job(0),
    jobs_done(0),
    dead_slaves(0)
{
  this->jobs = new int*[this->num_jobs];
  for (int i = 0; i < this->num_jobs; i++)
  {
    this->jobs[i] = new int[this->job_size];
    for (int j = 0; j < this->job_size; j++)
      this->jobs[i][j] = (i + 1) * (this->job_size - j);
  }

  this->slaves = new int[num_slaves];
}

Master::~Master()
{
  for (int i = 0; i < this->num_jobs; i++)
    delete[] this->jobs[i];
  delete[] this->jobs;
  delete[] this->slaves;
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
  int* data = new int[job_size]();
  MPI_Status status;

  while(hasWaitingJobs() or hasWorkingJobs() or hasSlavesAlive())
  {
    // Espera alguma mensagem
    MPI_Recv(data, job_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == TAG_IM_FREE)
    {
      if (hasWaitingJobs())
      {
        // Envia um novo job para aquele cara;
        MPI_Send(this->jobs[next_job], job_size, MPI_INT, status.MPI_SOURCE, TAG_NEW_JOB, MPI_COMM_WORLD);
        slaves[status.MPI_SOURCE] = next_job++;
      }
      else
      {
        // Não tem mais jobs! Mata o escravo!
        MPI_Send(data, job_size, MPI_INT, status.MPI_SOURCE, TAG_SUICIDE, MPI_COMM_WORLD);
        dead_slaves++;
      }
    }
    else if (status.MPI_TAG == TAG_JOB_DONE)
    {
      int jid = slaves[status.MPI_SOURCE];
      memcpy(this->jobs[jid], data, sizeof(int) * this->job_size);
      jobs_done++;
    }
  }

  delete[] data;
}