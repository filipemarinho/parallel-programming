/*

Foi criado um script para ler e executar todos os arquivos testes e o tempo de execução foi cronometrado 
utilizando o comando 'time' para servir de métrica para comparação
Foram obtidos:

Antes da paralelização (trabalho 1)
    real    13m16,279s  //wall clock time
    user    13m12,636s  //total user time, including all threads
    sys     0m3,568s    //time spent in kernel, IOs...

Depois da paralelização:
    real    5m56,421s
    user    14m13,169s
    sys     0m3,946s

Analisando o tempo 'real' ( que seria o T_p serial/parallel runtime) vemos que o tempo 
de execução diminui bastante, utilizando 8 cores o speedup foi de 2,2 em média
Comparando o tempo de 'user' (que seria o T_all tempo total) vemos que o tempo total de 
calculo permaneceu próximo ao valor anterior, ou seja, o overhead foi bem controlado.
O total overhead foi de 56s nesse caso.

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <omp.h>

typedef std::vector<std::vector<int>> matrix_t;

using namespace std;

enum Error
{
    SUCCESS,
    BAD_ARGUMENT,
    BAD_FILE,
    BAD_RESULT
};
class Graph
{
    public:
        int nVertex, nEdges;
        vector<int> vertA, vertB;
        matrix_t adjList;
        vector<int> degrees;
        int maxDegree = 0;
        vector<float> rks;

        // Lê o arquivo de entrada que contém o tamanho e as arestas do grafo
        void read(string filename);

        // Computa a lista de adj do grafo
        void getAdjList();

        // Computa o grau de cada nó
        void getGraphDegree();

        // Computa o coef. de clube dos ricos do grafo para o grau 0 até o grau máximo - 1
        void getRichClubCoef();

        // Armazena os resultados no arquivo de saida com a extensão .rcb
        void printResult(string filenameOutput);
};

int main(int argc, char *argv[])
{
    string filename = argv[1];

    cout << std::setprecision(5) << std::fixed;

    //Lê e inicializa o grafo
    Graph g1;
    g1.read(filename);

    //Cronometrando tempo de execução
    auto t1 = std::chrono::high_resolution_clock::now();

    //Calcula o coeficiente
    g1.getAdjList();
    g1.getGraphDegree();
    g1.getRichClubCoef();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto dif = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    cout << "Total elasped time " << dif << " seconds." << endl;

    g1.printResult(filename);

    return SUCCESS;
}

void Graph::read(string filename)
{
    // Abre o arquivo para leitura
    ifstream inputFile(filename);

    // Verifica a extensão
    if (!inputFile.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net")
    {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        exit(BAD_FILE);
    }

    // Lê o numero de Vertices e Arestas
    inputFile >> this->nVertex >> this->nEdges;

    /* Reservar evita que o vector tenha que ser realocado dinâmicamente diversas vezes */
    // Reserva espaço para alocar todas as arestas do grafo
    this->vertA.reserve(this->nEdges);
    this->vertB.reserve(this->nEdges);

    //Le as arestas
    for (int i = 0; i < this->nEdges; i++)
    {
        int tempA, tempB;
        inputFile >> tempA >> tempB;
        this->vertA.emplace_back(tempA);
        this->vertB.emplace_back(tempB);
    }

    inputFile.close();
    return;
}

void Graph::getAdjList()
{
    /* Para implementação dos grafos foi escolhida a lista de adj por lidar bem com grafos esparsos
    ao contrário da matriz de adj que requer NxN elementos para representação mesmo que existam poucas ligações
    e por facilitar o acesso aos vizinhos do vértice.
    */

    matrix_t adjList_(this->nVertex);

    // Para cada vertice atualiza a lista de adj do nó correspondente
    for (size_t i = 0; i < this->vertA.size(); i++)
    {
        adjList_[this->vertA[i]].emplace_back(this->vertB[i]);
        adjList_[this->vertB[i]].emplace_back(this->vertA[i]);
    }

    this->adjList = adjList_;

    return;
}

void Graph::getGraphDegree()
{

    degrees.resize(this->nVertex, 0);

    // Calcula o grau de cada nó e obtem o grau máximo
    for (auto i = 0; i < this->adjList.size(); i++)
    {
        degrees[i] = (int)this->adjList[i].size();

       if (degrees[i] > this->maxDegree) /*Obter o grau máximo aqui reduz um loop na lista de graus*/
           this->maxDegree = degrees[i];
    }

    return;
}

void Graph::getRichClubCoef()
{

    int maxDegree = this->maxDegree;
    vector<float> _rks(maxDegree, 0.0); // Garante que o tamanho não seja alterado dinamicamente
    vector<int> degrees = this->degrees;

    #pragma omp parallel for default(none) shared(degrees, adjList, _rks, maxDegree)
    for (int k = 0; k < maxDegree; k++) // Para cada k até k_max-1 calcula o coef. do clube dos ricos
    {
        float rk = 0.;   // Coeficiente de clube dos ricos
        int nk = 0;
        int i;

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        //CUIDADO: fors aninhados tem que ser explicitamente habilitados no openmp
        
        #pragma omp parallel for default(none) shared(degrees,k) \
            private(i) reduction(+:rk) reduction(+:nk)
        for (i = 0; i < degrees.size(); i++)
        {
            if (degrees[i] > k)
            {
                
                // Armazena o numero total de membros do clube dos ricos 
                nk += 1;

                // Procura na lista de adj por conexões que tenham grau maior que k
                std::for_each(adjList[i].begin(), adjList[i].end(), [&](auto &item) -> void
                              {
                                  if (degrees[item] > k)
                                      // Calculo o somatório contando cada vizinho que também participa do clube
                                      rk += 1;
                              });
            }
        }

        if (nk > 1){
            rk /= (float)(nk * (nk - 1.));
        }
        else{
            rk = 1;
        }

        _rks[k] = rk;
    }

    this->rks = _rks;
    return;
}

void Graph::printResult(string filename)
{

    // Muda a extensão
    filename.resize(filename.size() - 3);
    filename += "rcb";
    ofstream outputFile(filename, ios::trunc); //Cria ou sobrescreve o arquivo de resultados
    cout << "Arquivo de saida: " << filename << endl;
    if (!outputFile.is_open())
    {
        cerr << "Could not open the output file: " << filename << endl;
        exit(BAD_ARGUMENT);
    }

    outputFile << fixed << setprecision(5);

    for (size_t i = 0; i < this->rks.size(); i++)
    {
        outputFile << rks[i] << endl;
    }
    outputFile.close();
    return;
}       
