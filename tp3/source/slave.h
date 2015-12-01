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
  int job_num;  // número do job que está trabalhando
  void askForJob(void); // pede um job pro master
  void receiveJob(int*, MPI_Status*); // recebe um job do master
  void doJob(int*); // executa o job
  void sendResultsToMaster(int*); // devolve o resultado para o master

  bool keepRunning();

  bool must_die;
};
