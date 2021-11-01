//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
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

    // Computa o coef. de clube dos do grafo para o grau 0 até o grau máximo - 1
    void getRichClubCoef();

    // Armazena os resultados no arquivo de saida
    void printResult(string filenameOutput);
};

int main(int argc, char *argv[])
{
    //Automatizando testes:
    double total = 0;
    int inter = 0;
    vector<string> names = {"small-1-00", "small-2-00", "medium-1-00", "medium-2-00", "large-1-00", "large-2-00", "huge-1-00", "huge-2-00"}; //
    for (auto _name : names)
    {
        string name;
        for (int n = 0; n < 10; ++n)
        {
            name = _name + to_string(n) + ".net";

            cout << std::setprecision(5) << std::fixed << name << endl;
            string filename = "rich-club-tests/" + name;

            Graph g1;

            g1.read(filename);

            //Cronometrando tempo de execução
            auto t1 = std::chrono::high_resolution_clock::now();

            g1.getAdjList();
            g1.getGraphDegree();
            g1.getRichClubCoef();

            auto t2 = std::chrono::high_resolution_clock::now();
            auto dif = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
            total += dif;
            inter += 1;
            cout << "Elasped time " << dif << " seconds." << endl;

            string id = name;
            id.resize(id.size() - 3); //remove extensão
            g1.printResult(id);

            //Compara os resultados
            string expectedFilename = "rich-club-tests/expected-" + id + "rcb";
            ifstream resultFile(expectedFilename);
            if (!resultFile.is_open())
            {
                cerr << "Could not open the results file" << endl;
                exit(BAD_ARGUMENT);
            }

            float expected = -1.;
            int i = 0;
            while (resultFile >> expected)
            {
                if (abs(expected - g1.rks[i]) > .00001)
                {
                    cerr << "Bad Result! r(" << i << ") =" << g1.rks[i] << " expected " << expected << endl;
                    exit(BAD_RESULT);
                }
                i++;
            }
            cout << "Sucess" << endl;
        }
    }
    cout << "Total time " << total << " Mean time " << total / inter << endl;
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

    // Le o numero de Vertices e Arestas
    inputFile >> this->nVertex >> this->nEdges;
    cout << "Reading graph with " << this->nVertex << " vertices and " << this->nEdges << " arestas." << endl;

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
        // cout << "Aresta: " << this->vertA[i] << "," << this->vertB[i] << endl;
    }

    inputFile.close();
    return;
}

void Graph::getAdjList()
{
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
        degrees[i] = this->adjList[i].size();

        if (degrees[i] > this->maxDegree) //Obter o grau máximo aqui reduz um loop na lista de graus
            this->maxDegree = degrees[i];
    }

    return;
}

void Graph::getRichClubCoef()
{
    for (int k = 0; k < this->maxDegree; k++) // Para cada k até k_max -1 calculo o coef. do clube dos ricos
    {
        vector<int> R_k;
        float rk = 0.;
        float _rk = 0;

        // Acha os nós com grau > k e conta os vizinhos com grau > k
        for (size_t i = 0; i < this->degrees.size(); i++)
        {
            if (this->degrees[i] > k)
            {
                // Armazena no clube dos ricos o vertice
                R_k.emplace_back(i);

                //Procura na lista de adj por conexões que tenham grau maior que k
                std::for_each(adjList[i].begin(), adjList[i].end(), [&](auto &item) -> void
                              {
                                  if (this->degrees[item] > k)
                                      rk += 1;
                              });
            }
        }

        int nk = R_k.size();

        if (nk > 1)
        {
            rk /= (nk * (nk - 1.));
        }
        else
        {
            rk = 1;
        }

        this->rks.emplace_back(rk);
    }
    return;
}

void Graph::printResult(string filenameOutput)
{

    // ofstream outputFile(filenameOutput); //TO DO:
    filenameOutput +="rcb";

    ofstream outputFile("result.rcb", ios::trunc);
    if (!outputFile.is_open())
    {
        cerr << "Could not open the output file" << endl;
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