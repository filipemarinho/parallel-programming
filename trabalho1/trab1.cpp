//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// #include <iterator>
#include <algorithm>

typedef std::vector<std::vector<int>> matrix_t;

using namespace std;

//Print Matrix to cout
void printMatrix(matrix_t matrix);

//Return true if the vector has the element
bool hasElement(vector<int> v, int element);

//Return the adj list of a graph based on the vertex list
matrix_t listAdj(int size, vector<int> vert0, vector<int> vert1);

//Return the adj matrix of a graph based on the vertex list
matrix_t matrixAdj(int size, vector<int> vert0, vector<int> vert1);

//Return the graph degree for each node
vector<int> graphDegree(matrix_t matrix);

//Return the graph rich club coefficient for a degree k 
int graphRCCoef(matrix_t matrix);

void graphRichness(matrix_t adjList){

    vector<int> nodesDegree(adjList.size(), 0);

    cout << "Graph Degree" << endl;
    for (auto i = 0; i < adjList.size(); i++){
        cout << "Degree of " << i << ": " << adjList[i].size() << endl;
        nodesDegree[i] = adjList[i].size();
    }

    vector<int> R_k;

    return;
}

int main(int argc, char *argv[])
{
    string filename = argv[1];

    // Read from the text file
    ifstream inputFile(filename);
    if (!inputFile.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net")
    {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    int nVert, nArestas;
    inputFile >> nVert >> nArestas;
    cout << "Reading graph with " << nVert << " vertices and " << nArestas << " arestas." << endl;

    vector<int> vertA(nArestas);
    vector<int> vertB(nArestas);

    //Lê todas as arestas do arquivo
    for (int i = 0; i < nArestas; i++)
    {
        inputFile >> vertA[i] >> vertB[i];
        cout << "Aresta: " << vertA[i] << "," << vertB[i] << endl;
    }
    cout << "File read!" << endl
         << "\n";
    inputFile.close();

    //sorting the initial edges list would optmize caching in element acess

    matrix_t adjList = listAdj(nVert, vertA, vertB);
    matrix_t adjMatrix = matrixAdj(nVert, vertA, vertB);

    vector<int> degrees =  graphDegree(adjList);

    int coef0 = graphRCCoef(adjList);

    return 0;
}

matrix_t listAdj(int size, vector<int> vert0, vector<int> vert1)
{
    matrix_t adjList(size);

    for (int i = 0; i < vert0.size(); i++)
    {
        //cout << "Linha: " << i << ", Aresta: " << vert0[i] << "," << vert1[i] << endl;
        adjList[vert0[i]].push_back(vert1[i]);
        adjList[vert1[i]].push_back(vert0[i]);
    }

    cout << "List Adj Result" << endl;
    printMatrix(adjList);
    return adjList;
}

matrix_t matrixAdj(int size, vector<int> vert0, vector<int> vert1)
{
    matrix_t adjMatrix(size, vector<int>(size, 0));

    for (int i = 0; i < vert0.size(); i++)
    {
        adjMatrix[vert0[i]][vert1[i]] = 1;
        adjMatrix[vert1[i]][vert0[i]] = 1;
    }

    cout << "Matrix Adj Result" << endl;
    printMatrix(adjMatrix);

    return adjMatrix;
}

vector<int> graphDegree(matrix_t matrix){

    vector<int> nodesDegree(matrix.size(), 0);

    cout << "Graph Degree" << endl;
    for (auto i = 0; i < matrix.size(); i++){
        cout << "Degree of " << i << ": " << matrix[i].size() << endl;
        nodesDegree[i] = matrix[i].size();
    }

    return nodesDegree;
}

int graphRCCoef(matrix_t adjList){

    vector<int> degrees = graphDegree(adjList);
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

    cout << "Graph Rich Club Coefficient for k = " << k << ", n_k = " << nk << endl 
        << "R_k = " << endl;

    float rk = 0.;

    for (auto i = 0; i < R_k.size(); i++){
        cout << R_k[i] << ", ";
        for (auto j = 0; j < R_k.size(); j++){
        if (i != j && hasElement(adjList[R_k[i]], j)) ++rk;
        }
    }
    cout << endl << rk << endl;

    rk /= (nk*(nk-1.));
    if (nk <= 1) rk = 1; //return rk = 1
    cout << "r(" << k <<") = " << rk << endl<< endl;
    
    }
    return 0;
}

bool hasElement(vector<int> v, int element){
    bool hasFound = false;
    if(std::find(v.begin(), v.end(), element) != v.end())
    {
        //element exists in the vector
        hasFound = true;
    } 
    return hasFound;
}

void printMatrix(matrix_t matrix)
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