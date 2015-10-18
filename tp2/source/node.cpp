#include <stdio.h>
#include <algorithm>
#include "node.h"
#include "mpi.h"

Node::Node(int proc_n, int rank, int conquer_at, int conquer_local_size) 
          : proc_n(proc_n), rank(rank), conquer_at(conquer_at), conquer_local_size(conquer_local_size) {}

Node::~Node() {}

void Node::sort(vector<int>& numbers)
{
  //printf("%d recebeu %lu números\n", rank, numbers.size());
  // se o tamanho do vetor atende ao critério de conquista...
  if (canConquer(numbers))
    conquer(numbers);
  else if(conquer_local_size == 0)
    divide_v1(numbers);
  else
    divide_v2(numbers);
}

void Node::sort()
{
  vector<int> numbers;
  // espera receber um vetor do nodo pai
  receiveFromNode(parentRank(), numbers);
  // ordena utilizando divisão ou conquista
  sort(numbers);
  // envia o resultado para o nodo pai
  sendToNode(parentRank(), numbers);
}

// divide o vetor entre os dois filhos
void Node::divide_v1(vector<int>& numbers)
{
  //printf("%d vai dividir %lu números\n", rank, numbers.size());
  // calcula o rank do filho à esquerda
  const int left_rank  = rank * 2 + 1;
  // calcula o rank do filho à direita
  const int right_rank = rank * 2 + 2;

  vector<int> resto(numbers.begin() + conquer_at + 1, numbers.end());
  

  // printf("%d pegou %lu números para si e sobraram %lu\n", rank, meu.size(), resto.size());

  // calcula a posição da metade do vetor
  const size_t half_size = resto.size() / 2;

  // cria dois vetores com cada metade do vetor original
  vector<int> left(resto.begin(), resto.begin() + half_size);
  vector<int> right(resto.begin() + half_size, resto.end());

  // printf("%d vai enviar %lu números para %d e %lu para %d\n", rank, left.size(), left_rank, right.size(), right_rank);

  // envia os vetores para os nodos filhos
  sendToNode(left_rank, left);
  sendToNode(right_rank, right);

  // espera um retorno dos nodos filhos
  receiveFromNode(left_rank, left);
  receiveFromNode(right_rank, right);

  // mescla os retornos dos nodos filhos no vetor original
  mergeVectors(left, right, resto);
}

// pega um percentual do vetor para si e o restante distribui entre os filhos
void Node::divide_v2(vector<int>& numbers)
{
  //printf("%d vai dividir %lu números\n", rank, numbers.size());
  // calcula o rank do filho à esquerda
  const int left_rank  = rank * 2 + 1;
  // calcula o rank do filho à direita
  const int right_rank = rank * 2 + 2;

  vector<int> meu(numbers.begin(), numbers.begin() + conquer_local_size + 1);
  vector<int> resto(numbers.begin() + conquer_local_size + 1, numbers.end());
  

  // printf("%d pegou %lu números para si e sobraram %lu\n", rank, meu.size(), resto.size());

  // calcula a posição da metade do vetor
  const size_t half_size = resto.size() / 2;

  // cria dois vetores com cada metade do vetor original
  vector<int> left(resto.begin(), resto.begin() + half_size);
  vector<int> right(resto.begin() + half_size, resto.end());

  // printf("%d vai enviar %lu números para %d e %lu para %d\n", rank, left.size(), left_rank, right.size(), right_rank);

  // envia os vetores para os nodos filhos
  sendToNode(left_rank, left);
  sendToNode(right_rank, right);

  
  conquer(meu);
  

  // espera um retorno dos nodos filhos
  receiveFromNode(left_rank, left);
  receiveFromNode(right_rank, right);

  // mescla os retornos dos nodos filhos no vetor original
  mergeVectors(left, right, resto);

  // mescla o meu com o novo resto
 
  mergeVectors(meu, resto, numbers);
  
}

void Node::conquer(vector<int>& numbers)
{
  //printf("%d vai conquistar %lu números\n", rank, numbers.size());
  // método fornecido pelo professor para o bubblesort
  int c = 0, troca, trocou = 1;
  while (c < (numbers.size() - 1) & trocou)
  {
    trocou = 0;
    for (int d = 0 ; d < numbers.size() - c - 1; d++)
    {
      if (numbers[d] > numbers[d + 1])
      {
        troca        = numbers[d];
        numbers[d]   = numbers[d+1];
        numbers[d + 1] = troca;
        trocou = 1;
      }
    }
    c++;
  }
}

void Node::sendToNode(int destination, vector<int> vector)
{
  // envia o vetor para o destino especificado
  MPI_Send(&vector[0], vector.size(), MPI_INT, destination, 0, MPI_COMM_WORLD);
}

void Node::receiveFromNode(int source, vector<int>& vector)
{
  MPI_Status status;
  int amount = 0;

  // roda o probe
  MPI_Probe(source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  // verifica o tamanho da mensagem recebida (em inteiros)
  MPI_Get_count(&status, MPI_INT, &amount);
  // realoca a memória do vetor, adaptando-se ao tamanho da mensagem
  vector.resize(amount);
  // recebe os dados da mensagem no vetor realocado
  MPI_Recv(&vector[0], amount, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void Node::mergeVectors(vector<int> a, vector<int> b, vector<int>& c)
{
  // faz o merge de dois vetores resultando em um terceiro
  merge(a.begin(), a.end(), b.begin(), b.end(), c.begin());
}

bool Node::canConquer(vector<int> numbers)
{
  if (rank >= proc_n / 2) return true; // se é folha SEMPRE conquista
  return numbers.size() <= conquer_at;
}

int Node::parentRank()
{
  // auxiliar para calcular o rank do nodo pai
  int dec = rank % 2 != 0 ? 1 : 2;
  return (rank - dec) / 2;
}
