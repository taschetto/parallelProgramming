#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "master.h"
#include "slave.h"

main(int argc, char** argv)
{
  int my_rank;
  int proc_n;
  double t1, t2;

  MPI_Init(&argc , &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

  if (my_rank == 0)
  {
    t1 = MPI_Wtime();
    Master master(proc_n - 1, proc_n * 2, 30);
    //printf("Before:\n");
    //master.printJobs();
    master.mainLoop();
    //printf("After:\n");
    //master.printJobs();
    t2 = MPI_Wtime();
    printf("time elapsed: %f\n", t2 - t1);
  }
  else
  {
    Slave slave(30);
    slave.mainLoop();
  }

  MPI_Finalize();
}