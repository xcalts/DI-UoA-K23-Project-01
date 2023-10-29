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
lsh -i input.dat -q query.dat -o results.txt -k 15 -L 7 -N 5
lsh -i input.dat -q query.dat -o results.txt -k 10 -L 3 -R 0.5

Note:
The input and query files should be in the MNIST format with vector data.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;
    string query_file;
    string output_file;
    int no_hash_functions;
    int no_hash_tables;
    int no_nearest;
    int radius;

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

    LSH lsh = LSH(input.GetImages(), query.GetImages(), output_file, no_hash_functions, no_hash_tables, radius);

    ofstream output(output_file, ios::out | ios::trunc);

    if (output.is_open())
    {
        for (Image query_image : query.GetImages())
        {
            output << "===============================================================" << endl;
            output << "Query: " << query_image.GetIndex() << endl;
            output << query_image.Print();

            /* Find N approximately Nearest Neighbors using LSH. */
            const clock_t lsh_start = clock();
            set<Image, ImageComparator> lsh_nn_aprox = lsh.FindAproximateNearestNeighbors(no_nearest, query_image);
            const clock_t lsh_end = clock();
            double lsh_time = double(lsh_end - lsh_start) / CLOCKS_PER_SEC;
            output << "tLSH: " << lsh_time << endl;

            /* Find N real Nearest Neighbors using Brute Force. */
            const clock_t brute_start = clock();
            set<Image, ImageComparator> lsh_nn_brute = lsh.BruteForceNearestNeighbors(no_nearest, query_image);
            const clock_t brute_end = clock();
            double brute_time = double(brute_end - brute_start) / CLOCKS_PER_SEC;
            output << "tTrue: " << brute_time << endl;

            /* Comparison between LSH and Brute Force. */
            int i = 1;
            for (auto it1 = lsh_nn_aprox.begin(), it2 = lsh_nn_brute.begin();
                 (it1 != lsh_nn_aprox.end()) && (it2 != lsh_nn_brute.end());
                 ++it1, ++it2)
            {
                Image neighbor_lsh = *it1;
                Image neighbor_brute = *it2;

                output << "Nearest Neighbor-" << i << ": " << neighbor_lsh.GetIndex() << endl;
                output << "distanceLSH: " << neighbor_lsh.GetDist() << endl;
                output << "distanceTrue: " << neighbor_brute.GetDist() << endl;
                output << neighbor_lsh.Print();
                i++;
            }

            /* Find the Neighbors inside the radius. */
            set<Image, ImageComparator> neighbors_in_radius = lsh.RadiusSearch(query_image);
            output << "R-near neighbors:" << endl;
            for (set<Image, ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
            {
                Image neighbor = *it;
                output << neighbor.GetIndex() << endl;
                output << neighbor.Print();
            }

            output << "===============================================================" << endl;
        }

        output.close();
    }
    else
    {
        cout << "Failed to write to output file." << endl;
    }

    return EXIT_SUCCESS;
}