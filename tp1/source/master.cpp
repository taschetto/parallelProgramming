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

  // alocação dinâmica da estrutura auxiliar
  this->slaves = new int[num_slaves];
}

Master::~Master()
{
  // libera a memória alocada para os jobs e estrutura auxiliar
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
  MPI_Status status;

  // aloca memória para um buffer do tamanho de um job
  int* buffer = new int[job_size]();

  while(hasWaitingJobs() or hasWorkingJobs() or hasSlavesAlive())
  {
    // Recebe uma mensagem qualquer
    MPI_Recv(buffer, job_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG)
    {
      case TAG_IM_FREE: // um escravo sinalizou que está disponível
      {
        if (hasWaitingJobs())
        {
          // envia um novo job para este escravo e associa o escravo ao job enviado
          MPI_Send(this->jobs[next_job], job_size, MPI_INT, status.MPI_SOURCE, TAG_NEW_JOB, MPI_COMM_WORLD);
          slaves[status.MPI_SOURCE] = next_job++;
        }
        else
        {
          // embora o escravo esteja disponǘeil, não há mais jobs - logo, ele deve MORRER :D
          MPI_Send(buffer, job_size, MPI_INT, status.MPI_SOURCE, TAG_SUICIDE, MPI_COMM_WORLD);
          dead_slaves++;
        }
      } break;

      case TAG_JOB_DONE: // um escravo terminou de processar um job e enviou o resultado
      {
        // atualiza o job original com os dados do resultado e contabiliza a conclusão
        memcpy(this->jobs[slaves[status.MPI_SOURCE]], buffer, sizeof(int) * this->job_size);
        jobs_done++;
      } break;
    }
  }

  // libera a memória alocada para o buffer
  delete[] buffer;
}