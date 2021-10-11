// Execute: ./trab1 graph.net

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    string filename = argv[1];

    if(filename.substr(filename.find_last_of(".") + 1) == "net") {
    std::cout << "Nome do arquivo: " << filename << std::endl;
    } else {
        std::cout << "No..." << std::endl;
        return 0;
    }

    // Read Line
    string line;

    // Read from the text file
    ifstream file(filename);

    // Number of vertices
    getline(file, line);
    int n_v = std::stoi(line);

    //Number of arestas
    getline(file, line);
    int n_a = std::stoi(line);

    cout << "Numero de vertices: " << n_v << ", Numero de arestas: " << n_a << endl;

    //Identificadores de arestas
    for (int i = 0; i < n_v; i++)
    {
        getline(file, line);
        cout << "linha " << i << ": " << line[0] << "," << line[2] << endl;
    }

    // Close the file
    file.close();
    return 0;
}
