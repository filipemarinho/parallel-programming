//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <time.h>

typedef std::vector<std::vector<int>> matrix_t;
enum Error { SUCCESS, BAD_ARGUMENT, BAD_FILE };
using namespace std;    
class Graph {
    public:
    
    int nVertex, nEdges;
    vector<int> vertA, vertB;
    matrix_t adjList;
    vector<int> degrees;

    //Return the adj list of a graph based on the vertex list
    void getAdjList();

    // Read the input file and set graph
    void read(string filename);

    //Return the graph degree for each node
    void getGraphDegree();

    //Return the graph rich club coefficient for a degree k 
    void getRichClubCoef();

    //Print Matrix to cout
    void printMatrix(matrix_t matrix);

    //Return true if the vector has the element
    bool hasElement(vector<int> v, int element);
};



void Graph::read(string filename){
        // Read from the text file
    ifstream inputFile(filename);
    if (!inputFile.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net")
    {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        exit(BAD_ARGUMENT);
    }

    inputFile >> this->nVertex >> this->nEdges;
    cout << "Reading graph with " << this->nVertex << " vertices and " << this->nEdges << " arestas." << endl;

    this->vertA.resize(this->nEdges, 0);
    this->vertB.resize(this->nEdges,0);

    //Lê todas as arestas do arquivo
    for (int i = 0; i < this->nEdges; i++)
    {
        inputFile >> this->vertA[i] >> this->vertB[i];
        // cout << "Aresta: " << this->vertA[i] << "," << this->vertB[i] << endl;
    }
    cout << "File read!" << endl
         << "\n";
    inputFile.close();

    return;
}


int main(int argc, char *argv[])
{
    cout << std::setprecision(5) << std::fixed;
    string filename = argv[1];

    Graph g1;

    g1.read(filename);

    time_t start,end;
    time (&start);

    g1.getAdjList();
    g1.getGraphDegree();
    g1.getRichClubCoef();

    time (&end);
    double dif = difftime (end,start);
    cout << "Elasped time " << dif << " seconds."  << endl;

    return 0;
}


void Graph::getAdjList()
{
    matrix_t adjList_(this->nEdges);
    for (int i = 0; i < this->vertA.size(); i++)
    {
        // cout << "Linha: " << i << ", Aresta: " << this->vertA[i] << "," << this->vertB[i] << endl;
        adjList_[this->vertA[i]].push_back(this->vertB[i]);
        adjList_[this->vertB[i]].push_back(this->vertA[i]);
    }   
    this->adjList = adjList_;
    // cout << "List Adj Result" << endl;
    this->printMatrix(this->adjList);
    return;
}


void Graph::getGraphDegree(){

    degrees.resize(this->adjList.size(), 0);

    // cout << "Graph Degree" << endl;
    for (auto i = 0; i < this->adjList.size(); i++){
        // cout << "Degree of " << i << ": " << this->adjList[i].size() << endl;
        degrees[i] = this->adjList[i].size();
    }

    return;
}

void Graph::getRichClubCoef(){
    
    for (int k = 0; k< *max_element(this->degrees.begin(), this->degrees.end()); k++){
    // cout << "k = " << k << endl;
    //Guarda o conjunto de arestas conectadas ao nó que pertence a R(k)
    vector<int> R_k;
    for(int i = 0; i < this->degrees.size(); i++ ){
        if (this->degrees[i]>k) {
            R_k.push_back(i);
        }
    }

    int nk =  R_k.size();

    // cout << "Graph Rich Club Coefficient for k = " << k << ", n_k = " << nk << endl;

    float rk = 0.;

    for (int i = 0; i < R_k.size(); i++){
        for (int j = 0; j < R_k.size(); j++){
        if (hasElement(adjList[R_k[i]], R_k[j])) rk += 1.;
        }
    }

    rk /= (nk*(nk-1.));
    if (nk <= 1) rk = 1;
    cout << "r(" << k <<") = " << rk << endl;
    
    }
    return ;
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