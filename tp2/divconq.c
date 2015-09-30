#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define DELTA 10

/* buble sort */
void bs(int n, int * vector)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vector[d] > vector[d+1])
                {
                troca      = vector[d];
                vector[d]   = vector[d+1];
                vector[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

/* recebe um ponteiro para um vector que contem as mensagens recebidas dos filhos e            */
/* intercala estes valores em um terceiro vector auxiliar. Devolve um ponteiro para este vector */
 
int *interleaving(int vector[], int tam)
{
    int *vector_aux;
    int i1, i2, i_aux;

    vector_aux = (int *)malloc(sizeof(int) * tam);

    i1 = 0;
    i2 = tam / 2;

    for (i_aux = 0; i_aux < tam; i_aux++) {
        if (((vector[i1] <= vector[i2]) && (i1 < (tam / 2)))
            || (i2 == tam))
            vector_aux[i_aux] = vector[i1++];
        else
            vector_aux[i_aux] = vector[i2++];
    }

    return vector_aux;
}

int main()
{
    int i;
    int my_rank;       // Identificador deste processo
    int proc_n;        // Numero de processos disparados pelo usuario na linha de comando (np)       
    MPI_Status status; // estrutura que guarda o estado de retorno          

    MPI_Init(&argc , &argv); // funcao que inicializa o MPI, todo o codigo paralelo estah abaixo

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

    int *buffer;

    int father = 0;
    int i;

    int local_task_size, task_size, l_offset = 0, r_offset;
    int *result_from_left, *result_from_right;

    total_size = ARRAY_SIZE;
    
    if(my_rank == 0) {

        buffer = (int*)malloc(sizeof(int) * ARRAY_SIZE);

        local_task_size = ARRAY_SIZE;
        for (i=0 ; i<ARRAY_SIZE; i++)              /* inicializa array */
            buffer[i] = ARRAY_SIZE-i;
    } 
    else {

        /* MPI_Probe, neste caso é utilizado para testar o tamanho da mensagem a ser recebida */
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // apenas le a mensagem, nao a recebe
        MPI_Get_count(&status, MPI_INT, &local_task_size); //pega total de elementos passados e armazena em size

        father = status.MPI_source;
        buffer = (int*)malloc(sizeof(int) * local_task_size);

        MPI_Recv(buffer, local_task_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if(local_task_size <= DELTA) {
        bs(local_task_size, vetor);
    } else {
        task_size = local_task_size/2;  // tamanho da tarefa a repassar
        r_offset = (total_size - task_size);  // right_offset: marca o inicio da tarefa a ser enviada para direita 

        MPI_Send(buffer, task_size, MPI_INT, (my_rank *2 + 1), MPI_ANY_TAG, MPI_COMM_WORLD); // envia para esquerda
        MPI_Recv(result_from_left, task_size, MPI_INT, (my_rank *2 + 2), MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Send(buffer + r_offset, task_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD);  // envia para direita
        MPI_Recv(result_from_right, task_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int *result = (int*)malloc(sizeof(int) * local_task_size);
        memcpy(&(result[l_offset]), src, task_size);
        memcpy(&(result[r_offset]), src, task_size);

        buffer = interleaving(buffer, local_task_size);
    }

    return 0;
}
