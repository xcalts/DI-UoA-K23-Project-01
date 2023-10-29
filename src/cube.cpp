#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "mnist.h"
#include "cube.h"

using namespace std;

#define N_DEFAULT 1
#define R_DEFAULT 10000
#define DIMENSIONS_DEFAULT 14
#define M_DEFAULT 10
#define PROBES_DEFAULT 2

#pragma region HELP_MESSAGE
const char *help_msg = R"""(
Hypercube Algorithm for Vectors in d-Space

Usage:
cube [options]

Options:
-h, --help                   Print the help message.
-i, --input <input_file>     Input MNIST format file containing data vectors.
-q, --query <query_file>     Query MNIST format file for nearest neighbor search.
-o, --output <output_file>   Output file to store the results.
-N, --num-nearest <N>        Number of nearest points to search for (default: 1).
-R, --radius <R>             Search radius for range query (default: 10000).
-M, --candidates <M>         Max allowed number of edges of the hypercube.
-p  --probes                 
-k, --dimensions

Description:
This command line tool implements the Hypercube algorithm for vectors in d-space.
It can be used to find the nearest neighbors of a query vector or to perform range queries within a specified radius.

Example Usage:
cube -i input.dat -q query.dat -o results.txt -k 15 -L 7 -N 5
cube -i input.dat -q query.dat -o results.txt -k 10 -L 3 -R 0.5

Note:
The input and query files should be in the MNIST format with vector data.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;
    string query_file;
    string output_file;
    int no_nearest;
    int radius;
    int candidates;
    int probes;
    int dimensions;

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--radius"}, R_DEFAULT) >> radius;
    cmdl({"-M", "--candidates"}, M_DEFAULT) >> candidates;
    cmdl({"-probes", "--probes"}, PROBES_DEFAULT) >> probes;
    cmdl({"-k, --dimensions"}, DIMENSIONS_DEFAULT) >> dimensions;

    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);

    Hypercube hypercube = Hypercube(input.GetImages(), query.GetImages(), output_file, dimensions, candidates, probes, no_nearest, radius);

    cout << "== Input ==" << endl;
    input.Print();

    std::cout << "== Query ==" << std::endl;
    query.Print();

    std::cout << "== Hypercube ==" << std::endl;
    hypercube.Print();

    hypercube.Execute();

    return EXIT_SUCCESS;
}