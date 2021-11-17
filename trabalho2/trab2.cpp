//g++ -o trab2.o trab2.cpp && ./trab2.o graph.net && diff -w rich-club-results/ rich-club-expected/
//Testar com flags adicionais: -g -std=c++17 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]){
     
    int num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        cout << "Helllo Parallel! ";
        int thread_id = omp_get_thread_num();
        cout << "total: " << num_threads << " id: " << thread_id << endl;
    }

    
    return 0;
}