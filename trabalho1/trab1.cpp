//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

typedef std::vector<std::vector<int>> matrix_t; 

using namespace std;

matrix_t listAdj(int size, vector<int> vert0, vector<int> vert1)
{
    matrix_t adjList(size);

    for (int i = 0; i < vert0.size(); i++)
    {
        //cout << "Linha: " << i << ", Aresta: " << vert0[i] << "," << vert1[i] << endl;
        adjList[vert0[i]].push_back(vert1[i]);
        adjList[vert1[i]].push_back(vert0[i]);
    }

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

    // Print result 
    for (int i = 0; i < adjMatrix.size(); i++)
    {
        cout << "Adj Matrix line " << i << endl;
        for (int j = 0; j < adjMatrix[i].size(); j++)
        {
            cout << adjMatrix[i][j] << ", ";
        }
        cout << endl;
    }
    return adjMatrix;
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

    vector<int> vertA(nVert);
    vector<int> vertB(nVert);

    for (int i = 0; i < nVert; i++)
    { //usar int i = 0; i < vect.size(); i++ se quiser ter certeza que o elemento está no vetor
        inputFile >> vertA[i] >> vertB[i];
        //cout << "Aresta: " << vertA[i] << "," << vertB[i] << endl;
    }
    cout << "File read!" << endl << "\n";
    inputFile.close();

    matrix_t adjList = listAdj(nVert, vertA, vertB);
    matrix_t  adjMatrix = matrixAdj(nVert, vertA, vertB);


    return 0;
}
