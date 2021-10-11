#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


int main(int argc, char *argv[]){
    
    char *filename = argv[1];
    cout << "Nome do arquivo: " << filename  << endl;

    // Create a text string, which is used to output the line read
    string line;

    // Read from the text file
    ifstream file(filename);

    // Numero de vertices
    getline(file, line);
    int n_v = std::stoi(line);

    //Numero de arestas
    getline(file, line);
    int n_a = std::stoi(line);

    cout << "Numero de vertices: " << n_v << ", Numero de arestas: " << n_a << endl; 

    //Identificadores de arestas
    for (int i = 0; i<n_v; i++){
        getline(file, line);
        cout << "linha " << i << ": " << line[0] << "," <<line[2] << endl;
    }

    // Close the file
    file.close();
    return 0;
}

