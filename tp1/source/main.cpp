#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "master.h"
#include "slave.h"

main(int argc, char** argv)
{
  int my_rank, proc_n, job_count, job_size;
  double t1, t2;

  MPI_Init(&argc , &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

  job_count = atoi(argv[1]);
  job_size = atoi(argv[2]);

  if (my_rank == 0)
  {
    t1 = MPI_Wtime();
    Master master(my_rank, proc_n - 1, job_count, job_size);
    // printf("Before:\n");
    // master.printJobs();
    master.mainLoop();
    // printf("After:\n");
    //master.printJobs();
    t2 = MPI_Wtime();
    printf("time elapsed: %f\n", t2 - t1);
  }
  else
  {
    Slave slave(my_rank, job_size);
    slave.mainLoop();
  }

  MPI_Finalize();
}
