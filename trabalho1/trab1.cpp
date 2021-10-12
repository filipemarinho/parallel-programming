//g++ -o trab1.o trab1.cpp && ./trab1.o graph.net
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    string filename = argv[1];

    int vertA, vertB;
    // Read from the text file
    ifstream input_file(filename);
    if (!input_file.is_open() || filename.substr(filename.find_last_of(".") + 1) != "net") {
        cerr << "Could not open the file, check extension. Filename - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    int n_vertices, n_arestas;
    input_file >> n_vertices >> n_arestas;
    cout << "Reading graph with " << n_vertices << " vertices and " << n_arestas << " arestas." << endl;

    for (int i = 0; i < n_vertices; i++){
        int vert_a, vert_b;
        input_file >> vert_a >> vert_b;
        cout << "Aresta: " << vert_a << "," << vert_b << endl;
    }

    input_file.close();

    return 0;
}
