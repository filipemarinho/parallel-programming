//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

typedef std::vector<std::vector<int>> adj_list_t; 

using namespace std;

adj_list_t listAdj(int size, vector<int> vert0, vector<int> vert1)
{
    adj_list_t adjList(size);

    for (int i = 0; i < vert0.size(); i++)
    {
        //cout << "Linha: " << i << ", Aresta: " << vert0[i] << "," << vert1[i] << endl;
        adjList[vert0[i]].push_back(vert1[i]);
        adjList[vert1[i]].push_back(vert0[i]);
    }

    // Print result 
    for (int i = 0; i < adjList.size(); i++)
    {
        cout << "Adj List of element " << i << endl;
        for (int j = 0; j < adjList[i].size(); j++)
        {
            cout << adjList[i][j] << ", ";
        }
        cout << endl;
    }

    return adjList;
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

    adj_list_t adjList = listAdj(nVert, vertA, vertB);

    inputFile.close();

    return 0;
}
