//g++ -o trab1.o trab1.cpp && ./trab1.o rich-club-results/small-1-001.net && diff -w rich-club-results/medium-1-001.rcb rich-club-expected/medium-1-001.rcb 
//Testar com flags adicionais: -g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
/*
np = 2
real    9m27,862s
user    17m11,299s
sys     0m17,614s

np = 4
real    10m22,855s
user    35m57,465s
sys     0m34,143s

np = 8
real    15m26,972s
user    70m16,303s
sys     38m19,185s

real    13m33,779s
user    66m20,910s
sys     33m29,364s

real    14m5,722s
user    67m35,047s
sys     36m25,092s

np = 16
real    20m58,988s
user    88m17,842s
sys     66m57,755s

without  Rk
real    21m21,862s
user    89m57,968s
sys     68m25,752s

-o2, np = 16
real    8m31,697s
user    35m47,770s
sys     25m27,927s

real    8m20,293s
user    34m48,595s
sys     24m54,379s
-o2, np = 8
real    4m55,619s
user    22m26,317s
sys     11m35,146s

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <mpi.h>
#include <stdio.h>

// #include <algorithm>
#include <numeric>

typedef std::vector<std::vector<int>> matrix_t;

enum Error
{
    SUCCESS,
    BAD_ARGUMENT,
    BAD_FILE,
    BAD_RESULT
};

using namespace std;

// Lê o arquivo de entrada que contém o tamanho e as arestas do grafo
void read(string filename, int &nVertex, int &nEdges, vector<int> &vertA, vector<int> &vertB);

// Armazena os resultados no arquivo de saida com a extensão .rcb
void printResult(string filenameOutput, vector<float> rks);

void compute_partition(int N, int p, vector<int> &count, vector<int> &offset) {
  int q = N / p;
  int r = N % p;
  int curr_off = 0;
  for (int i = 0; i < p; ++i) {
    count[i] = i < r ? q : q+1+r;
    offset[i] = curr_off;
    curr_off += count[i];
  }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int nprocs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    string filename = argv[1];

    //Lê e inicializa o grafo
    int nVertex, nEdges;
    vector<int> vertA, vertB;

    if (rank == 0) read(filename, nVertex, nEdges, vertA, vertB);
    int parallelThreshold = 1000;


    MPI_Bcast(&nVertex, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank !=0){
    vertA.reserve(nEdges);
    vertB.reserve(nEdges);
    }
    MPI_Bcast(&vertA[0], nEdges, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vertB[0], nEdges, MPI_INT, 0, MPI_COMM_WORLD);

    //Cronometrando tempo de execução
    auto t1 = std::chrono::high_resolution_clock::now();

    /* Para implementação dos grafos foi escolhida a lista de adj por lidar bem com grafos esparsos
    ao contrário da matriz de adj que requer NxN elementos para representação mesmo que existam poucas ligações
    e por facilitar o acesso aos vizinhos do vértice.
    */
    // Obtendo a lista de adj do grafo
    vector<vector<int>> adjList(nVertex);
    // Para cada vertice atualiza a lista de adj do nó correspondente
    for (int i = 0; i < nEdges; i++)
    {
        adjList[vertA[i]].emplace_back(vertB[i]);
        adjList[vertB[i]].emplace_back(vertA[i]);
    }
    // Obtendo o grau de cada nó
    vector<int> degrees(nVertex, 0);
    int maxDegree = 0;
    // Calculo do grau de cada nó e o grau máximo
    for (auto i = 0; i < nVertex; i++)
    {
        degrees[i] = (int) adjList[i].size();

        if (degrees[i] == 0) {
            printf("Strange... \n Process %d of %d: has degrees[%d] = %d.\n", rank, nprocs, i, degrees[i]);
        }
        //Obter o grau máximo aqui reduz um loop na lista de graus
        if (degrees[i] > maxDegree) 
            maxDegree = degrees[i];
    }
    if (maxDegree == 0) {
        printf("Bad max degree :C");
        MPI_Finalize();
        exit(BAD_ARGUMENT);
    }

    // vector<float> partial_rk_list;
    // vector<int> counts(nprocs), displs(nprocs);
    vector<float> partial_rk_list;
    vector<int> count(nprocs), offset(nprocs);
    int N = maxDegree;
    compute_partition(N, nprocs, count, offset);


    //Divisão dos elementos para os os processos
    int kmax = maxDegree;                       //kmax = n_elem
    // int q = kmax / (nprocs);                      // Quociente
    // int r = kmax % (nprocs);                      // Resto
    // auto first_q = fill_n(begin(counts), r, q + 1); // Q + 1 elementos no primeiro contador
    // printf("rank = %d: ", rank);
    // for (int x = 0; x < nprocs; x++){
    //     printf("counts[%d] = %d ", x, counts[x]);
    // }
    // fill(first_q, end(counts), q);                  // Q elementos no resto
    // for (int x = 0; x < nprocs; x++){
    //     printf("counts_a[%d] = %d ", x, counts[x]);
    // }
    // printf("rank = %d: \n", rank);
    // displs[0] = 0;
    // partial_sum(begin(counts), end(counts) - 1, begin(displs) + 1); // Preenche displs com o índice que cada proc deve iniciar
    // if (rank ==0 )printf("kmax = %d, q = %d , r = %d \n", kmax, q, r);
    // for (int x = 0; x < nprocs; x++){
    //     printf("displs[%d] = %d, ", x, displs[x]);
    //     printf("counts [%d] = %d, ", x, counts[x]);
    //     // printf("displs[%d] = k = %d, r = %d , q*x = %d\n", x, novo, rank, q*x);
    // }
    // printf("rank = %d: \n", rank);
    // Calculo do coef. de clube dos ricos do grafo para o grau 0 até o grau máximo - 1
    // vector<float> rks(maxDegree, 0.0); // Garante que o tamanho não seja alterado dinamicamente
    // printf("Process %d of %d: has rks = %d .\n", rank, nprocs, maxDegree);
    // printf("Process %d of %d: has maxDegree = %d, rks.size = %d .\n", rank, nprocs, maxDegree, rks.size());
    //Calcula o coeficiente
    // for (int k = 0; k < maxDegree; k++) // Para cada k até k_max -1 calcula o coef. do clube dos ricos
        //For levando em conta a divisão dos passos para o calculo do rcc
    // for (int k = displs[rank]+ counts[rank]; k < displs[rank] ; k--)
    for (int k = offset[rank]; k < offset[rank] + count[rank] ; k++)
    {
        // vector<int> R_k; // Armazena o lista de vertices do clube dos ricos
        float rk = 0.;   // Coeficiente de clube dos ricos
        int nk = 0;

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        for (size_t i = 0; i < nVertex; i++)
        {
            if (degrees[i] > k)
            {
                // Armazena o vértice no clube dos ricos 
                // R_k.emplace_back(i);
                nk += 1;

                /* Procurar iterativamente na lista de adj ao incluir um vértice foi a melhor alternativa 
                encontrada para calcular o valor do somatorio do coef. rk,
                pois principalmente para k pequeno o custo de percorrer Rk é muito maior que percorrer a lista de adj do vértice. */
                // Procura na lista de adj por conexões que tenham grau maior que k
                std::for_each(adjList[i].begin(), adjList[i].end(), [&](auto &item) -> void
                              {
                                  if (degrees[item] > k)
                                      rk += 1;
                              });
            }
        }

        // int nk = (int) R_k.size();
        // if (nk > 1)
        //     rk /= (float)(nk * (nk - 1.));
        // else
        //     rk = 1;
        if (nk > 1){
            rk /= (float)(nk * (nk - 1.));
        }
        else{
            rk = 1;
        }

        // rks[k] = rk;
        partial_rk_list.push_back(rk);
        printf("Process %d of %d: has k = %d, rk = %.5f, kmax = %d.\n", rank, nprocs, k, rk, kmax);
    }

    MPI_Barrier( MPI_COMM_WORLD);

    // Transferencia dos dados para o rank 0
    vector<float> rks = vector<float>(kmax); // Variavel real
    // MPI_Gatherv(&partial_rk_list[0], counts[rank], MPI_FLOAT, &rks[0], &counts[0], &displs[0], MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Gatherv(&partial_rk_list[0], count[rank], MPI_FLOAT, &rks[0], &count[0], &offset[0], MPI_FLOAT, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        auto t2 = std::chrono::high_resolution_clock::now();
        auto dif = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
        cout << "Elasped time " << dif << " seconds." << endl;
        printResult(filename, rks);
    }
    MPI_Finalize();
    return SUCCESS;
}

void read(string filename, int &nVertex, int &nEdges, vector<int> &vertA, vector<int> &vertB)
{
    // Abre o arquivo para leitura
    ifstream inputFile(filename);

    // Verifica a extensão
    if (!inputFile.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net")
    {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        MPI_Finalize();
        exit(BAD_ARGUMENT);
    }

    // Lê o numero de Vertices e Arestas
    inputFile >> nVertex >> nEdges;

    /* Reservar evita que o vector tenha que ser realocado dinâmicamente diversas vezes */
    // Reserva espaço para alocar todas as arestas do grafo
    vertA.reserve(nEdges);
    vertB.reserve(nEdges);

    //Le as arestas
    for (int i = 0; i < nEdges; i++)
    {
        int tempA, tempB;
        inputFile >> tempA >> tempB;
        vertA.emplace_back(tempA);
        vertB.emplace_back(tempB);
    }

    inputFile.close();
    return;
}


void printResult(string filename, vector<float> rks)
{
    cout << "Done writing " << filename << endl;
    // Muda a extensão
    filename.resize(filename.size() - 3);
    filename += "rcb";
    ofstream outputFile(filename, ios::trunc); //Cria ou sobrescreve o arquivo de resultados
    if (!outputFile.is_open())
    {
        cerr << "Could not open the output file: " << filename << endl;
        MPI_Finalize();
        exit(BAD_FILE);
    }

    outputFile << fixed << setprecision(5);

    for (size_t i = 0; i < rks.size(); i++)
    {
        outputFile << rks[i] << endl;
    }
    outputFile.close();
    return;
}