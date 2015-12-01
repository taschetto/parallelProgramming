#pragma once

#include <stdio.h>
#include "mpi.h"

class Slave
{
public:
  Slave(int, int);
  ~Slave();
  void mainLoop();

private:
  int rank;     // rank do processo
  int job_size; // tamanho de cada job
  void askForJob(void); // pede um job pro master
  int receiveJob(int*, MPI_Status*); // recebe um job do master
  void doJob(int*); // executa o job
  void sendResultsToMaster(int*, int); // devolve o resultado para o master
};
