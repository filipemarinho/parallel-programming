// mpic++ -g -std=c++17 -O2 -Wpedantic -Wall -Wextra -o trab3.o trab3.cpp && mpirun -oversubscribe -np 5 ./trab3.o ../../../rich-club-results/huge-1-000.net
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <mpi.h>

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
    MPI_Init(&argc, &argv); // antes de usar agrc e argv, pode alterar esses argumentos para utilização

    int nprocs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    printf("Process %d of %d is running.\n", rank, nprocs);

    std::chrono::high_resolution_clock::time_point t1,t2; 
    cout << std::setprecision(5) << std::fixed;


    string filename = argv[1];
    Graph g1;

    if (rank == 0){
    g1.read(filename);
    }

    MPI_Bcast(&g1.nVertex, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Process %d of %d: has nVertex = %d.\n", rank, nprocs, g1.nVertex);

    MPI_Bcast(&g1.nEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Process %d of %d: has nEdges = %d.\n", rank, nprocs, g1.nEdges);

    if (rank !=0){
    g1.vertA.reserve(g1.nEdges);
    g1.vertB.reserve(g1.nEdges);
    }
    MPI_Bcast(&g1.vertA[0], g1.nEdges, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&g1.vertB[0], g1.nEdges, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Process %d of %d: after vertA[0] = %d .\n", rank, nprocs, g1.vertA[0]);
    printf("Process %d of %d: after vertB[0] = %d .\n", rank, nprocs, g1.vertB[0]);

    // //Cronometrando tempo de execução
    t1 = std::chrono::high_resolution_clock::now();

    g1.getAdjList();
    g1.getGraphDegree();
    // g1.getRichClubCoef();

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0){
    t2 = std::chrono::high_resolution_clock::now();
    auto dif = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    printf("Total elasped time: %d seconds\n", dif);

    // g1.printResult(filename);
    }
    
    MPI_Finalize();
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
        MPI_Finalize();
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

    printf("Read graph from %s \n with %d vertex and %d edges \n", filename.c_str(), this->nVertex, this->nEdges);

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
    this->degrees.resize(this->nVertex, 0);
    // Calcula o grau de cada nó e obtem o grau máximo
    for (auto i = 0; i < this->adjList.size(); i++)
    {
        this->degrees[i] = (int)this->adjList[i].size();
       if (this->degrees[i] > this->maxDegree) /*Obter o grau máximo aqui reduz um loop na lista de graus*/
           this->maxDegree = this->degrees[i];
    }
    return;
}

void Graph::getRichClubCoef()
{

    // int maxDegree = this->maxDegree;
    vector<float> _rks(maxDegree, 0.0); // Garante que o tamanho não seja alterado dinamicamente
    // vector<int> degrees = this->degrees;

    // //  shared(degrees, adjList, _rks, maxDegree)
    for (int k = 0; k < maxDegree; k++) // Para cada k até k_max-1 calcula o coef. do clube dos ricos
    { 

        float rk = 0.;   // Coeficiente de clube dos ricos
        int nk = 0;
        int i;

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        //  shared(degrees,k) private(i) reduction(+:rk) reduction(+:nk)
        for (i = 0; i < degrees.size(); i++)
        {
            if (degrees[i] > k)
            {
                
                // Armazena o numero total de membros do clube dos ricos 
                nk += 1;

                /* Procurar iterativamente na lista de adj ao incluir um vértice foi a melhor alternativa 
                encontrada para calcular o valor do somatorio do coef. rk, pois principalmente para k 
                pequeno o custo de percorrer Rk é muito maior que percorrer a lista de adj do vértice. */
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
    // float *data = NULL;
    // if (rank==0) data = (float *) malloc(nprocs)*sizeof(float); 
    // MPI_Gather(&rk, 2, MPI_FLOAT, &data, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    // this->rks = _rks;
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
        MPI_Finalize();
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