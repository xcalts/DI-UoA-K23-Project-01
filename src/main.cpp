#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "file.h"
#include "mnist.h"

int main(int, char *argv[])
{
    // -i data/t10k-images.idx3-ubyte;
    argh::parser cmdl(argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    std::string input_file = cmdl("-i", "--input-file").str(); // -i data/t10k-images.idx3-ubyte;
    std::string query_file = cmdl("-q", "--query-file").str();
    std::string k = cmdl("-k", "--number-of-lsh").str();
    std::string L = cmdl("-L", "--number-of-hash-tables").str();
    std::string output_file = cmdl("-o", "--output-file").str();
    std::string N = cmdl("-N", "--number-of-nearest").str();
    std::string R = cmdl("-R", "--radius").str();

    MNIST mnist = MNIST(input_file);

    mnist.Print();

    return EXIT_SUCCESS;
}