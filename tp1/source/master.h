#pragma once

#include <stdio.h>

class Master
{
public:
  Master(int num_slaves, int num_jobs, int job_size);
  ~Master();

  void printJobs();
  void mainLoop();

private:
  int** jobs;       // estrutura para armazenar jobs
  int* slaves;      // estrutura para associar jobs a slaves
  int num_jobs;     // número total de jobs
  int job_size;     // tamanho de cada job
  int next_job;     // índice do próximo job
  int jobs_done;    // contador de jobs concluídos
  int num_slaves;   // total de processos slaves criados
  int dead_slaves;  // total de processos slaves destruídos

  bool hasWaitingJobs() { return next_job < num_jobs; }
  bool hasWorkingJobs() { return jobs_done < num_jobs; }
  bool hasSlavesAlive() { return dead_slaves < num_slaves; }
};