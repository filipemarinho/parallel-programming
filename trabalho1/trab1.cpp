//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net && diff -w rich-club-results/ rich-club-expected/
//Testar com -g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>

typedef std::vector<std::vector<int>> matrix_t;

enum Error
{
    SUCCESS,
    BAD_ARGUMENT,
    BAD_FILE,
    BAD_RESULT
};

using namespace std;
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
    cout << "Elasped time " << dif << " seconds." << endl;

    g1.printResult(filename);

    return 0;
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
        exit(BAD_ARGUMENT);
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

    // Para cada vertice atualiza a lista de adj dos nós correspondente
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
    for (int k = 0; k < this->maxDegree; k++) // Para cada k até k_max -1 calcula o coef. do clube dos ricos
    {
        vector<int> R_k; // Armazena o lista de vertices do clube dos ricos
        float rk = 0.;   // Coeficiente de clube dos ricos

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        for (size_t i = 0; i < this->degrees.size(); i++)
        {
            if (this->degrees[i] > k)
            {
                // Armazena no clube dos ricos o vertice
                R_k.emplace_back(i);

                /* Procurar iterativamente na lista de adj ao incluir um vértice foi a melhor alternativa encontrada para calcular o valor do somatorio do coef. rk,
                pois principalmente para k pequeno o custo de percorrer Rk é muito maior que percorrer a lista de adj do vértice. */
                // Procura na lista de adj por conexões que tenham grau maior que k
                std::for_each(adjList[i].begin(), adjList[i].end(), [&](auto &item) -> void
                              {
                                  if (this->degrees[item] > k)
                                      rk += 1;
                              });
            }
        }

        int nk = (int) R_k.size();

        if (nk > 1)
            rk /= (float)(nk * (nk - 1.));
        else
            rk = 1;

        this->rks.emplace_back(rk);
    }

    return;
}

void Graph::printResult(string filename)
{

    // Muda a extensão
    filename.resize(filename.size() - 3);
    filename += "rcb";
    ofstream outputFile(filename, ios::trunc); //Cria ou sobrescreve o arquivo de resultados
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