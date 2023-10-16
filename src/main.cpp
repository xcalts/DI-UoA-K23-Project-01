#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "file.h"
#include "mnist.h"
#include "lsh.h"

int main(int, char *argv[])
{
    // -i data/t10k-images.idx3-ubyte;
    argh::parser cmdl(argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    std::string input_file = cmdl("-i", "--input-file").str(); // -i data/t10k-images.idx3-ubyte;
    std::string query_file = cmdl("-q", "--query-file").str();
    const std::string k = cmdl("-k", "--number-of-hashing-functions").str();
    const std::string L = cmdl("-L", "--number-of-hash-tables").str();
    const std::string output_file = cmdl("-o", "--output-file").str();
    const std::string N = cmdl("-N", "--number-of-nearest").str();
    const std::string R = cmdl("-R", "--radius").str();

    MNIST mnist = MNIST(input_file);

    mnist.Print();

    LSH lsh = LSH(query_file, output_file, atoi(k.c_str()), atoi(L.c_str()), atoi(N.c_str()), atoi(R.c_str()), mnist.GetImages());

    lsh.Print();

    return EXIT_SUCCESS;
}