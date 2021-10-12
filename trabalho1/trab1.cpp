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

    return 0;
}
