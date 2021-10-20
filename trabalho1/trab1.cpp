//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// #include <iterator>
#include <algorithm>
#include <iomanip>

typedef std::vector<std::vector<int>> matrix_t;
enum Error { SUCCESS, BAD_ARGUMENT, BAD_FILE };

using namespace std;
class Graph{
    public:

    Graph(string filename);

    int nVert, nArestas;

    matrix_t adjList;
    matrix_t edgeList;

    vector<int> vertA, vertB;

    //Return the adj list of a graph based on the vertex list
    matrix_t listAdj();

    //Return the adj matrix of a graph based on the vertex list
    matrix_t matrixAdj(vector<int> vert0, vector<int> vert1);

    //Return the graph degree for each node
    vector<int> graphDegree(matrix_t matrix);

    //Return the graph rich club coefficient for a degree k 
    int graphRCCoef(matrix_t matrix);

    //Print Matrix to cout
    void printMatrix(matrix_t matrix);

    //Return true if the vector has the element
    bool hasElement(vector<int> v, int element);

};

int main(int argc, char *argv[])
{
    string filename = "graph.net";//argv[1];
    Graph graph(filename);
    graph.graphRCCoef(graph.listAdj());
    

    // //sorting the initial edges list would optmize caching in element acess
    return 0;
}

Graph::Graph(string filename){

    // Open the text file and check extension
    ifstream inputFile(filename);
    if (!inputFile.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net")
    {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        exit(BAD_ARGUMENT);
    }

    // Read number of edges and vertex
    inputFile >> this->nVert >> this->nArestas;
    // cout << "Reading graph with " << this->nVert << " vertices and " << this->nArestas << " arestas." << endl;

    vector<int> vertA(this->nArestas);
    vector<int> vertB(this->nArestas);

    //Lê todas as arestas do arquivo
    for (int i = 0; i < this->nArestas; i++)
    {
        inputFile >> vertA[i] >> vertB[i];
        // cout << "Aresta: " << vertA[i] << "," << vertB[i] << endl;
    }
    cout << "File read!" << endl
         << "\n";
    inputFile.close();

    this->vertA = vertA;
    this->vertB = vertB;

    this->adjList = this->listAdj();
    this->printMatrix(this->adjList);
}

matrix_t Graph::listAdj()
{
    matrix_t adjList(this->nVert);

    for (int i = 0; i < this->vertA.size(); i++)
    {
        //cout << "Linha: " << i << ", Aresta: " << this->vertA[i] << "," << this->vertB[i] << endl;
        adjList[this->vertA[i]].push_back(this->vertB[i]);
        adjList[this->vertB[i]].push_back(this->vertA[i]);
    }

    //cout << "List Adj Result" << endl;
    //printMatrix(adjList);
    return adjList;
}

matrix_t Graph::matrixAdj(vector<int> vert0, vector<int> vert1)
{
    matrix_t adjMatrix(this->nVert, vector<int>(this->nVert, 0));

    for (int i = 0; i < vert0.size(); i++)
    {
        adjMatrix[vert0[i]][vert1[i]] = 1;
        adjMatrix[vert1[i]][vert0[i]] = 1;
    }

    // cout << "Matrix Adj Result" << endl;
    // printMatrix(adjMatrix);

    return adjMatrix;
}

vector<int> Graph::graphDegree(matrix_t matrix){

    vector<int> nodesDegree(matrix.size(), 0);

    // cout << "Graph Degree" << endl;
    for (auto i = 0; i < matrix.size(); i++){
        // cout << "Degree of " << i << ": " << matrix[i].size() << endl;
        nodesDegree[i] = matrix[i].size();
    }

    return nodesDegree;
}

int Graph::graphRCCoef(matrix_t adjList){
    cout << std::setprecision(5) << std::fixed;

    vector<int> degrees = this->graphDegree(adjList);
    for (int k = 0; k< *max_element(degrees.begin(), degrees.end()); k++){
    cout << "k = " << k << endl;
    //Guarda o conjunto de arestas conectadas ao nó que pertence a R(k)
    vector<int> R_k;
    for(int i = 0; i < degrees.size(); i++ ){
        if (degrees[i]>k) {
            R_k.push_back(i);
        }
    }

    int nk =  R_k.size();

    cout << "Graph Rich Club Coefficient for k = " << k << ", n_k = " << nk << endl;

    float rk = 0.;

    for (auto i = 0; i < R_k.size(); i++){
        for (auto j = 0; j < R_k.size(); j++){
        if (this->hasElement(adjList[R_k[i]], R_k[j])) rk += 1.;
        }
    }

    rk /= (nk*(nk-1.));
    if (nk <= 1) rk = 1;
    cout << "r(" << k <<") = " << rk << endl;
    
    }
    return 0;
}

bool Graph::hasElement(vector<int> v, int element){
    bool hasFound = false;
    if(std::find(v.begin(), v.end(), element) != v.end())
    {
        //element exists in the vector
        hasFound = true;
    } 
    return hasFound;
}

void Graph::printMatrix(matrix_t matrix)
{
    for (auto i = 0; i < matrix.size(); i++)
    {
        cout << "Matrix line " << i << endl;
        for (int j = 0; j < matrix[i].size(); j++)
        {
            cout << matrix[i][j] << ", ";
        }
        cout << endl;
    }

    cout << endl;
}

