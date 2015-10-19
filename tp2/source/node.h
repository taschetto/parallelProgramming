#include <vector>

using namespace std;

class Node
{
public:
  Node(int, int, int, bool);
  ~Node();

  void sort(vector<int>&); // ordena um vetor
  void sort();             // espera receber um vetor e o ordena

private:
  void divide(vector<int>&);  // divide um vetor :P
  void conquer(vector<int>&); // conquista um vetor :P

  void sendToNode(int, vector<int>);  // envia um vetor para o destino especificado
  void receiveFromNode(int, vector<int>&); // recebe um vetor da fonte especificada
  void mergeVectors(vector<int>, vector<int>, vector<int>&); // mescla dois vetores

  bool canConquer(vector<int>); // auxiliar para definir se divide ou conquista
  bool hasLocalSort(); // auxiliar para definir se haverá local sort ou não
  int parentRank(); // auxiliar para calcular o rank do nodo pai

  int proc_n;       // número de processos
  int rank;         // rank do nodo
  int conquer_at;   // ponto de corte (critério para divisão e conquista)
  bool local_sort;  // indica se haverá sort local ou não
};