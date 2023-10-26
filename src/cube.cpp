#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "mnist.h"
#include "cube.h"

using namespace std;

#define K_DEFAULT 4
#define N_DEFAULT 1
#define R_DEFAULT 10000
#define DIMENSIONS_DEFAULT 14
#define M_DEFAULT 10
#define PROBES_DEFAULT 2

int main(int argc, char *argv[])
{
    string input_file;
    string query_file;
    string output_file;
    int no_hash_functions;
    int no_nearest;
    int radius;
    int candidates;
    int probes;
    int dimensions;

    const char *help_msg = R"""(

)""";

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-k", "--hash-functions"}, K_DEFAULT) >> no_hash_functions;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--radius"}, R_DEFAULT) >> radius;
    cmdl({"-M", "--candidates"}, M_DEFAULT) >> candidates;
    cmdl({"-probes", "--probes"}, PROBES_DEFAULT) >> probes;
    cmdl({"-kd, --dimensions"}, DIMENSIONS_DEFAULT) >> dimensions;

    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);

    Hypercube hypercube = Hypercube(input, query, output_file, dimensions, candidates, probes, no_nearest, radius);

    cout << "== Input ==" << endl;
    input.Print();

    std::cout << "== Query ==" << std::endl;
    query.Print();

    std::cout << "== Hypercube ==" << std::endl;
    hypercube.Print();

    hypercube.Execute();

    return EXIT_SUCCESS;
}