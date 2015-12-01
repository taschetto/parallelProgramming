#pragma once

#include <stdio.h>
#include "mpi.h"

class Master
{
public:
  Master(int, int, int, int);
  ~Master();

  void printJobs();
  void mainLoop();

private:
  int rank;          // rank do processo
  int** jobs;        // estrutura para armazenar jobs
  int num_jobs;      // número total de jobs
  int job_size;      // tamanho de cada job
  int next_job;      // índice do próximo job
  int jobs_done;     // contador de jobs concluídos
  int num_slaves;    // total de processos slaves criados
  int dead_slaves;   // total de processos slaves destruídos

  void receive(int*, MPI_Status*); // recebe uma mensagem qualquer em um buffer
  void sendJobToSlave(int); // envia o próximo job para o PID especificado
  void getResultsFromSlave(int, int*); // recebe um job concluído por um slave
  void killSlave(int); // ordena que o escravo especificado se suicide

  bool hasWaitingJobs() { return next_job < num_jobs; }
  bool hasWorkingJobs() { return jobs_done < num_jobs; }
  bool hasSlavesAlive() { return dead_slaves < num_slaves; }
};