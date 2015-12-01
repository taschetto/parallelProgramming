#pragma once

#include <stdio.h>
#include "mpi.h"

class Slave
{
public:
  Slave(int, int, int);
  ~Slave();
  void mainLoop();

private:
  int rank;         // rank do processo
  int thread_count; // quantidade de threads
  int job_size;     // tamanho de cada job
  void receiveJob(int*, MPI_Status*); // recebe um job do master
  void doJob(int*); // executa o job
  void sendResultsToMaster(int*); // devolve o resultado para o master

  bool must_die;    // indica se o slave deve morrer ou não
  bool mustRun();
};
