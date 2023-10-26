#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "mnist.h"
#include "lsh.h"

using namespace std;

#define K_DEFAULT 4
#define L_DEFAULT 5
#define N_DEFAULT 1
#define R_DEFAULT 10000

int main(int argc, char *argv[])
{
    string input_file;
    string query_file;
    string output_file;
    int no_hash_functions;
    int no_hash_tables;
    int no_nearest;
    int radius;

    const char *help_msg = R"""(
LSH Algorithm for Vectors in d-Space

Usage:
lsh_tool [options]c

Options:
-h, --help                   Print the help message.
-i, --input <input_file>     Input MNIST format file containing data vectors.
-q, --query <query_file>     Query MNIST format file for nearest neighbor search.
-o, --output <output_file>   Output file to store the results.
-k, --hash-functions <k>     Number of hash functions to use (default: 4).
-L, --hash-tables <L>        Number of hash tables to use (default: 5).
-N, --num-nearest <N>        Number of nearest points to search for (default: 1).
-R, --radius <R>             Search radius for range query (default: 10000).

Description:
This command line tool implements the Locality-Sensitive Hashing (LSH) algorithm for vectors in d-space.
It can be used to find the nearest neighbors of a query vector or to perform range queries within a specified radius.

Example Usage:
lsh_tool -i input.dat -q query.dat -o results.txt -k 15 -L 7 -N 5
lsh_tool -i input.dat -q query.dat -o results.txt -k 10 -L 3 -R 0.5

Note:
The input and query files should be in the MNIST format with vector data.
)""";

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-k", "--hash-functions"}, K_DEFAULT) >> no_hash_functions;
    cmdl({"-L", "--hash-tables"}, L_DEFAULT) >> no_hash_tables;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--radius"}, R_DEFAULT) >> radius;

    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);

    LSH lsh = LSH(input, query, output_file, no_hash_functions, no_hash_tables, no_nearest, radius);

    cout << "== Input ==" << endl;
    input.Print();

    std::cout << "== Query ==" << std::endl;
    query.Print();

    std::cout << "== LSH ==" << std::endl;
    lsh.Print();

    query.PrintImage(50);

    lsh.Execute();

    return EXIT_SUCCESS;
}