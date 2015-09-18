#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "master.h"
#include "slave.h"

main(int argc, char** argv)
{
  int my_rank; // identificador deste processo
  int proc_n;  // numero de processos disparados pelo usuario na linha de comando (np)

  MPI_Init(&argc , &argv);                 // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

  if (my_rank == 0)
  {
    Master master(proc_n - 1, proc_n * 2, 30);
    //printf("Before:\n");
    //master.printJobs();
    master.mainLoop();
    //printf("After:\n");
    //master.printJobs();
  }
  else
  {
    Slave slave(30);
    slave.mainLoop();
  }

  MPI_Finalize();
}