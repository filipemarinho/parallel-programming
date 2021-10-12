//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
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
    int number;
    int number1;
    // Read from the text file
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    while (input_file >> number >> number1) {
        cout << number << "; "<< number1 << "; " << endl;
    }
    cout << endl;
    input_file.close();

/* 
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
        int vert_a;
        sscanf(line.c_str(), "%d", &vert_a);
        int vert_b;
        sscanf(line.c_str(), "%d", &vert_b);
        cout << "linha " << i << ": " << vert_a << "," << vert_b << endl; //Fails with numbers > 9, 
    }

    // Close the file
    file.close(); */
    return 0;
}
