#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "argh.h"
#include "brute.h"
#include "lsh.h"
#include "mnist.h"

#define K_DEFAULT 4
#define L_DEFAULT 5
#define N_DEFAULT 1
#define R_DEFAULT 10000

using namespace std;

#pragma region HELP_MESSAGE
const char *help_msg = R"""(
LSH Algorithm for Vectors in d-Space

Usage:
lsh [options]

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
lsh -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -k 15 -L 10 -N 5 -R 5000

Note:
The input and query files should be in the MNIST format.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;     // Input MNIST format file containing data vectors.
    string query_file;     // Query MNIST format file for nearest neighbor search.
    string output_file;    // Output file to store the results.
    int no_hash_functions; // Number of hash functions to use (default: 4).
    int no_hash_tables;    // Number of hash tables to use (default: 5).
    int no_nearest;        // Number of nearest points to search for (default: 1).
    int radius;            // Search radius for range query (default: 10000).

    // Parse the command line arguments using the argh.h functionality.
    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-k", "--hash-functions"}, K_DEFAULT) >> no_hash_functions;
    cmdl({"-L", "--hash-tables"}, L_DEFAULT) >> no_hash_tables;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--radius"}, R_DEFAULT) >> radius;

    // In the following cases, print the help message.
    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Create the required class instances.
    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);
    LSH lsh = LSH(input, no_hash_functions, no_hash_tables);
    BRUTE bf = BRUTE(input);
    ofstream output(output_file, ios::out | ios::trunc);
    clock_t start, end;
    double time;

    // Print results in output file.
    if (output.is_open())
    {
        for (MNIST_Image query_image : query.GetImages())
        {
            output << "===" << endl;
            output << "Query: " << query_image.GetIndex() << endl;

            // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Locality-Sensitive Hashing.
            start = clock();
            set<MNIST_Image, MNIST_ImageComparator> lsh_nn = lsh.FindNearestNeighbors(no_nearest, query_image);
            end = clock();
            time = double(end - start) / CLOCKS_PER_SEC;
            output << "timeLSH: " << time << " // seconds" << endl;

            // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Brute Force.
            start = clock();
            set<MNIST_Image, MNIST_ImageComparator> brute_nn = bf.FindNearestNeighbors(no_nearest, query_image);
            end = clock();
            time = double(end - start) / CLOCKS_PER_SEC;
            output << "timeBRUTE: " << time << " // seconds" << endl;

            // Print Comparison Stats between LSH and Brute Force.
            int i = 1;
            for (auto it1 = lsh_nn.begin(), it2 = brute_nn.begin();
                 (it1 != lsh_nn.end()) && (it2 != brute_nn.end());
                 it1++, it2++)
            {
                MNIST_Image neighbor_lsh = *it1;
                MNIST_Image neighbor_brute = *it2;

                output << "NN-" << i << " Index: " << neighbor_lsh.GetIndex() << endl;
                output << "distanceLSH: " << neighbor_lsh.GetDist() << endl;
                output << "distanceBRUTE: " << neighbor_brute.GetDist() << endl;
                i++;
            }

            // Find the Neighbors inside the radius.
            set<MNIST_Image, MNIST_ImageComparator> neighbors_in_radius = lsh.RadiusSearch(query_image, radius);
            output << "Radius: " << radius << endl;
            for (auto it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
            {
                MNIST_Image neighbor = *it;

                output << neighbor.GetIndex() << endl;
            }
        }

        output.close();
    }
    else
    {
        cout << "Failed to write to output file." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}