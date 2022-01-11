//g++ -o trab1.o trab1.cpp && ./trab1.o rich-club-results/small-1-001.net && diff -w rich-club-results/medium-1-001.rcb rich-club-expected/medium-1-001.rcb 
//Testar com flags adicionais: -g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
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

// Lê o arquivo de entrada que contém o tamanho e as arestas do grafo
void read(string filename, int &nVertex, int &nEdges, vector<int> &vertA, vector<int> &vertB);
// Computa a lista de adj do grafo

// Computa o grau de cada nó

// Computa o coef. de clube dos ricos do grafo para o grau 0 até o grau máximo - 1


// Armazena os resultados no arquivo de saida com a extensão .rcb
void printResult(string filenameOutput, vector<float> rks);

int main(int argc, char *argv[])
{
    string filename = argv[1];

    cout << std::setprecision(5) << std::fixed;

    //Lê e inicializa o grafo
    int nVertex, nEdges;
    vector<int> vertA, vertB;

    read(filename, nVertex, nEdges, vertA, vertB);
    
    //Cronometrando tempo de execução
    auto t1 = std::chrono::high_resolution_clock::now();

    /* Para implementação dos grafos foi escolhida a lista de adj por lidar bem com grafos esparsos
    ao contrário da matriz de adj que requer NxN elementos para representação mesmo que existam poucas ligações
    e por facilitar o acesso aos vizinhos do vértice.
    */

    matrix_t adjList(nVertex);

    // Para cada vertice atualiza a lista de adj do nó correspondente
    for (size_t i = 0; i < vertA.size(); i++)
    {
        adjList[vertA[i]].emplace_back(vertB[i]);
        adjList[vertB[i]].emplace_back(vertA[i]);
    }

    vector<int> degrees(nVertex, 0);
    int maxDegree = 0;

    // Calcula o grau de cada nó e obtem o grau máximo
    for (auto i = 0; i < adjList.size(); i++)
    {
        degrees[i] = (int)adjList[i].size();
        if (degrees[i] > maxDegree) /*Obter o grau máximo aqui reduz um loop na lista de graus*/
            maxDegree = degrees[i];
    }
    
    vector<float> rks;

    //Calcula o coeficiente
    for (int k = 0; k < maxDegree; k++) // Para cada k até k_max -1 calcula o coef. do clube dos ricos
    {
        vector<int> R_k; // Armazena o lista de vertices do clube dos ricos
        float rk = 0.;   // Coeficiente de clube dos ricos

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        for (size_t i = 0; i < degrees.size(); i++)
        {
            if (degrees[i] > k)
            {
                // Armazena o vértice no clube dos ricos 
                R_k.emplace_back(i);

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

        int nk = (int) R_k.size();
        if (nk > 1)
            rk /= (float)(nk * (nk - 1.));
        else
            rk = 1;

        rks.emplace_back(rk);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto dif = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    cout << "Elasped time " << dif << " seconds." << endl;

    printResult(filename, rks);

    return 0;
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
        exit(BAD_ARGUMENT);
    }

    outputFile << fixed << setprecision(5);

    for (size_t i = 0; i < rks.size(); i++)
    {
        outputFile << rks[i] << endl;
    }
    outputFile.close();
    return;
}