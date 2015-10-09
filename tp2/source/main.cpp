#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "node.h"
#include "mpi.h"

using namespace std;

int main(int argc, char** argv)
{
  int my_rank, proc_n;
  double t1, t2;

  MPI_Init(&argc , &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

  if (proc_n != 1 && proc_n != 3 && proc_n != 7 && proc_n != 15 && proc_n != 31)
  {
    printf("NP must be 1, 3, 7, 15 or 31!\n");
    return -1;
  }

  const int size       = atoi(argv[1]);
  const int conquer_at = ceil(size / proc_n);

  if (my_rank == 0)
  {
    // inicializa o vetor a ser ordenado
    vector<int> numbers(size);
    for (int i = 0 ; i < size; i++) numbers[i] = size - i;

    t1 = MPI_Wtime();
    Node node(proc_n, my_rank, conquer_at);
    // nodo inicial recebe como entrada o vetor de inteiros para ordenar
    node.sort(numbers);
    t2 = MPI_Wtime();

    printf("time elapsed: %f\n", t2 - t1);
    //printf(" correctness: %s\n", is_sorted(numbers.begin(), numbers.end()) ? "true" : "false");
  }
  else
  {
    Node node(proc_n, my_rank, conquer_at);
    // os demais nodos recebem os vetores para ordenação através do MPI
    node.sort();
  }

  MPI_Finalize();
  return 0;
}
