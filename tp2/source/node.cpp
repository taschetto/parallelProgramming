#include <stdio.h>
#include <algorithm>
#include "node.h"
#include "mpi.h"

Node::Node(int rank, int conquer_at) : conquer_at(conquer_at), rank(rank) {}

Node::~Node() {}

void Node::sort(vector<int>& numbers)
{
  // se o tamanho do vetor atende ao critério de conquista...
  if (numbers.size() <= conquer_at)
    conquer(numbers);
  else
    divide(numbers);
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

void Node::divide(vector<int>& numbers)
{
  // calcula o rank do filho à esquerda
  const int left_rank  = rank * 2 + 1;
  // calcula o rank do filho à direita
  const int right_rank = rank * 2 + 2;
  // calcula a posição da metade do vetor
  const size_t half_size = numbers.size() / 2;

  // cria dois vetores com cada metade do vetor original
  vector<int> left(numbers.begin(), numbers.begin() + half_size);
  vector<int> right(numbers.begin() + half_size, numbers.end());

  // envia os vetores para os nodos filhos
  sendToNode(left_rank, left);
  sendToNode(right_rank, right);

  // espera um retorno dos nodos filhos
  receiveFromNode(left_rank, left);
  receiveFromNode(right_rank, right);

  // mescla os retornos dos nodos filhos no vetor original
  mergeVectors(left, right, numbers);
}

void Node::conquer(vector<int>& numbers)
{
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

int Node::parentRank()
{
  // auxiliar para calcular o rank do nodo pai
  int dec = rank % 2 != 0 ? 1 : 2;
  return (rank - dec) / 2;
}