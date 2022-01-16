/*
Filipe Antunes Marinho - 10438866
A paralelização foi feita com base na distribuição de dados dos problema, devido a independência dos calculos de cada um dos coeficientes r[k], foram distribuidos os indices k entre os processos disponiveis.
A disponibilização dos dados para os outros processos foi feita pelo broadcast dos vertices lidos do arquivo.
Isso pois a lista de vértice ao contrário da lista de adjacência tem tamanho bem definido e pode ser enviado por um único buffer. 
Além disso sabe-se do trabalho passado que o custo computacional da obtenção da lista de adj e de graus é bem menor que o do calculo do coeficiente em si.


Foi utilizado o mesmo bash script para ler e executar todos os arquivos testes e o tempo de execução foi cronometrado 
utilizando o comando 'time' para servir de métrica para comparação. Foram obtidos:

Serial:
    real    12m42,141s
    user    12m32,452s
    sys     0m4,239s

    Com -O2
    real    3m48,944s
    user    3m44,736s
    sys     0m4,167s 

MPI:
    np = 4
    real    9m3,818s
    user    33m26,608s
    sys     0m28,996s

    np = 4, Com -O2
    real    3m12,701s
    user    10m15,649s
    sys     0m26,558s

    np = 8 Com -O2
    real    5m11,522s
    user    22m33,834s
    sys     12m17,998s

OpenMP:
    NUM_THREADS = 4
    real    7m18,920s
    user    11m14,062s
    sys     0m4,376s

    NUM_THREADS = 4 e -O2
    real    1m51,823s
    user    2m30,075s
    sys     0m4,100s   

    NUM_THREADS = 8 e -O2
    real    1m44,760s
    user    3m8,601s
    sys     0m4,207s

Os resultados obtidos mostram desempenho melhor que o caso Serial, mas piores que com o uso do OpemMP
Existem problemas de escalabilidade nesse código. Possivelmente causados pelo desbalanceamento de carga
causados pela distribuição consecutiva dos indices k de grau para o calculo do coeficiente.
Mas uma distribuição de indices em ordem diferentes se mostrou muito complexa para recuperação dos valores de r[k].
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

// Particiona N valores em p processadores para calculo do k local
void compute_partition(int N, int p, vector<int> &count, vector<int> &offset);

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    string filename = argv[1];

    int nVertex, nEdges;
    vector<int> vertA, vertB;

    //Lê e inicializa o grafo no rank 0
    if (rank == 0)
        read(filename, nVertex, nEdges, vertA, vertB);

    //Transmite os dados lidos para os outros processos
    MPI_Bcast(&nVertex, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nEdges, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Reserva o espaço necessário nos outros processos
    if (rank != 0)
    {
        vertA.reserve(nEdges);
        vertB.reserve(nEdges);
    }
    // Transmissão da lista de vértices para todos os processos
    MPI_Bcast(&vertA[0], nEdges, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vertB[0], nEdges, MPI_INT, 0, MPI_COMM_WORLD);

    //Cronometrando tempo de execução
    auto t1 = std::chrono::high_resolution_clock::now();

    /* Para implementação dos grafos foi escolhida a lista de adj por lidar bem com grafos esparsos
    ao contrário da matriz de adj que requer NxN elementos para representação mesmo que existam poucas ligações
    e por facilitar o acesso aos vizinhos do vértice.
    */
    // Obtendo a lista de adj do grafo por cada processo
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
        degrees[i] = (int)adjList[i].size();

        //Obter o grau máximo aqui reduz um loop na lista de graus
        if (degrees[i] > maxDegree)
            maxDegree = degrees[i];
    }
    if (maxDegree == 0)
    {
        printf("Bad max degree");
        MPI_Finalize();
        exit(BAD_ARGUMENT);
    }

    // Distribuição dos k valores de grau para o calculo do coeficiente
    vector<float> partial_rk_list;
    vector<int> count(nprocs), offset(nprocs);
    int N = maxDegree;

    int parallelThreshold = 1000; //defini um limite minimo de graus para utilização de todos os processos paralelos
    if (maxDegree < parallelThreshold && nprocs > 2)
    {
        //Isso é necessário pois para k pequeno o custo das trasmissões dos dados é muito maior que o ganho com a paralelização
        compute_partition(N, 2, count, offset);
    }
    else
    {
        compute_partition(N, nprocs, count, offset);
    }
    // Entretanto a função compute_partions retornara as listas de indices locais em sequencia o que acaba causando um desbalanceamento de carga
    // Pois um processo encarregado dos indices k = {0,1,2} terá um tempo de execução muito maior do que um processo com k = {3,4,5}
    // Pois quanto menor o k mais elementos estão presentes no subgrafo

    // Calculo do coef. de clube dos ricos do grafo para o conjuntos de ks locais dados pela compute_partition
    for (int k = offset[rank]; k < offset[rank] + count[rank]; k++)
    {
        float rk = 0.; // Coeficiente de clube dos ricos
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

        if (nk > 1)
        {
            rk /= (float)(nk * (nk - 1.));
        }
        else
        {
            rk = 1;
        }

        partial_rk_list.push_back(rk);
    }

    MPI_Barrier(MPI_COMM_WORLD); //Sincroniza os processos

    // Recuperação dos dados no processo 0
    vector<float> rks = vector<float>(maxDegree); //Vetor de recebimento dos valores
    MPI_Gatherv(&partial_rk_list[0], count[rank], MPI_FLOAT, &rks[0], &count[0], &offset[0], MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        //Determinação do tempo de calculo
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

void compute_partition(int N, int p, vector<int> &count, vector<int> &offset)
{
    int q = N / (p);
    int r = N % (p);
    int curr_off = 0;
    for (int i = 0; i < p; ++i)
    {                                 //para cada rank
        count[i] = i < r ? q + 1 : q; //determina o numero de elementos
        offset[i] = curr_off;         //determina o indice local para cada rank
        curr_off += count[i];
    }
}