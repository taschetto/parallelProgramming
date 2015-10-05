#include <vector>

using namespace std;

class Node
{
public:
  Node(int, int);
  ~Node();

  void sort(vector<int>&); // ordena um vetor
  void sort();             // espera receber um vetor e o ordena

private:
  void divide(vector<int>&);  // divide um vetor :P
  void conquer(vector<int>&); // conquista um vetor :P

  void sendToNode(int, vector<int>);  // envia um vetor para o destino especificado
  void receiveFromNode(int, vector<int>&); // recebe um vetor da fonte especificada
  void mergeVectors(vector<int>, vector<int>, vector<int>&); // mescla dois vetores

  int parentRank(); // função auxiliar para calcular o rank do nodo pai

  int rank;         // rank do nodo
  int conquer_at;   // ponto de corte (critério para divisão e conquista)
};