#include "master.h"
#include "tags.h"
#include <stdio.h>
#include <cstring>

Master::Master(int rank, int num_slaves, int num_jobs, int job_size)
  : rank(rank),
    num_slaves(num_slaves),
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

  for (int i = 1; i <= this->num_slaves; i++) // inicia enviando uma rajada de jobs
  {
    if (hasWaitingJobs())
      sendJobToSlave(i);
  }

  while(hasWaitingJobs() or hasWorkingJobs() or hasSlavesAlive())
  {
    receive(buffer, &status);

    if (status.MPI_TAG == TAG_JOB_DONE) // um escravo terminou de processar um job e enviou o resultado
    {
      getResultsFromSlave(status.MPI_SOURCE, buffer);

      if (hasWaitingJobs())
        sendJobToSlave(status.MPI_SOURCE);
      else
        killSlave(status.MPI_SOURCE);
    }
    else
      killSlave(status.MPI_SOURCE); // Isto não deve ocorrer. Mas SE ocorrer, deve mandar o escravo se matar.
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
  MPI_Send(this->jobs[this->next_job], this->job_size, MPI_INT, slave_pid, TAG_NEW_JOB, MPI_COMM_WORLD);
  this->slaves[slave_pid] = this->next_job++;
}

void Master::getResultsFromSlave(int slave_pid, int* buffer)
{
  // atualiza o job original com os dados do resultado e contabiliza a conclusão
  memcpy(this->jobs[this->slaves[slave_pid]], buffer, sizeof(int) * this->job_size);
  this->jobs_done++;
}

void Master::killSlave(int slave_pid)
{
  // embora o escravo esteja disponível, não há mais jobs - logo, ele deve MORRER :D
  MPI_Send(0, 0, MPI_INT, slave_pid, TAG_SUICIDE, MPI_COMM_WORLD);
  this->dead_slaves++;
}
