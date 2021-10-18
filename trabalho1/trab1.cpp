//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

typedef std::vector<std::vector<int>> matrix_t;

using namespace std;

//Print Matrix to cout
void printMatrix(matrix_t matrix);

//Return the adj list of a graph based on the vertex list
matrix_t listAdj(int size, vector<int> vert0, vector<int> vert1);

//Return the adj matrix of a graph based on the vertex list
matrix_t matrixAdj(int size, vector<int> vert0, vector<int> vert1);

//Return the graph degree for each node
vector<int> graphDegree(matrix_t matrix);

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

    vector<int> vertA(nVert);
    vector<int> vertB(nVert);

    for (int i = 0; i < nVert; i++)
    { //usar int i = 0; i < vect.size(); i++ se quiser ter certeza que o elemento está no vetor
        inputFile >> vertA[i] >> vertB[i];
        //cout << "Aresta: " << vertA[i] << "," << vertB[i] << endl;
    }
    cout << "File read!" << endl
         << "\n";
    inputFile.close();

    //sorting the initial edges list would optmize caching in element acess

    matrix_t adjList = listAdj(nVert, vertA, vertB);
    matrix_t adjMatrix = matrixAdj(nVert, vertA, vertB);
    vector<int> degrees =  graphDegree(adjList);
    cout << "aaaaaaaaaaaaaaaaaaa" << endl;
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

    vector<int> nodesDegree(matrix.size());

    cout << "Graph Degree" << endl;
    for (auto i = 0; i < matrix.size(); i++){
        cout << "Degree of " << i << ": " << matrix[i].size() << endl;
        nodesDegree.push_back(matrix[i].size());
    }

    return nodesDegree;
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