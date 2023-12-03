#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "argh.h"
#include "brute.h"
#include "mnist.h"
#include "cube.h"
#include "misc.h"

#define N_DEFAULT 1
#define R_DEFAULT 10000
#define DIMENSIONS_DEFAULT 14
#define M_DEFAULT 10
#define PROBES_DEFAULT 2

using namespace std;

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
-M, --max-candidates <M>     Max allowed number of edges of the hypercube.
-p  --probes                 
-k, --dimensions

Description:
This command line tool implements the Hypercube algorithm for vectors in d-space.
It can be used to find the nearest neighbors of a query vector or to perform range queries within a specified radius.

Example Usage:
cube -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -o results.txt -M 10 -p 10 -k 14

Note:
The input and query files should be in the MNIST format with vector data.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;  // Input MNIST format file containing data vectors.
    string query_file;  // Query MNIST format file for nearest neighbor search.
    string output_file; // Output file to store the results.
    int no_nearest;     // Number of nearest points to search for (default: 1).
    int radius;         // Search radius for range query (default: 10000).
    int candidates;     // Max number of candinates.
    int probes;
    int dimensions;

    // Parse the command line arguments using the argh.h functionality.
    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--radius"}, R_DEFAULT) >> radius;
    cmdl({"-M", "--candidates"}, M_DEFAULT) >> candidates;
    cmdl({"-probes", "--probes"}, PROBES_DEFAULT) >> probes;
    cmdl({"-k, --dimensions"}, DIMENSIONS_DEFAULT) >> dimensions;

    // In the following cases, print the help message.
    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Create the required class instances.
    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);
    Hypercube hypercube = Hypercube(input, dimensions, candidates, probes);
    BRUTE bf = BRUTE(input);
    ofstream output(output_file, ios::out | ios::trunc);
    clock_t start, end;
    double time;
    double time_aprox_sum = 0;
    double time_brute_sum = 0;
    double max_maf = 0;

    // Print results in output file.
    if (output.is_open())
    {
        output << "CUBE Results" << endl;
        cout << "[i] Calculating Results" << endl;
        printProgress(0.0);
        output << "CUBE Results" << endl;
        for (MNIST_Image query_image : query.GetImages())
        {
            output << "====================================================================================" << endl;
            output << "Query: " << query_image.GetIndex() << endl;

            // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Locality-Sensitive Hashing.
            start = clock();
            set<MNIST_Image, MNIST_ImageComparator> hypercube_nn = hypercube.FindNearestNeighbors(no_nearest, query_image);
            end = clock();
            time = double(end - start) / CLOCKS_PER_SEC;
            time_aprox_sum += time;
            output << "timeCUBE: " << time << "s" << endl;

            // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Brute Force.
            start = clock();
            set<MNIST_Image, MNIST_ImageComparator> lsh_nn_brute = bf.FindNearestNeighbors(no_nearest, query_image);
            end = clock();
            time = double(end - start) / CLOCKS_PER_SEC;
            time_brute_sum += time;
            output << "timeBRUTE:  " << time << "s" << endl;

            // Print Comparison Stats between LSH and Brute Force.
            int i = 1;
            for (auto it1 = hypercube_nn.begin(), it2 = lsh_nn_brute.begin();
                 (it1 != hypercube_nn.end()) && (it2 != lsh_nn_brute.end());
                 it1++, it2++)
            {
                MNIST_Image neighbor_cube = *it1;
                MNIST_Image neighbor_brute = *it2;

                if (i == 1)
                {
                    double maf = static_cast<double>(neighbor_cube.GetDist()) / static_cast<double>(neighbor_brute.GetDist());
                    if (maf > max_maf)
                        max_maf = maf;
                }

                output << "NN--" << i << " Index: " << neighbor_cube.GetIndex() << endl;
                output << "distanceCUBE: " << neighbor_cube.GetDist() << endl;
                output << "distanceBRUTE: " << neighbor_brute.GetDist() << endl;
                i++;
            }

            // Find the Neighbors inside the radius.
            set<MNIST_Image, MNIST_ImageComparator> neighbors_in_radius = hypercube.RadiusSearch(query_image, radius);
            output << "Radius: " << radius << endl;
            for (set<MNIST_Image, MNIST_ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
            {
                MNIST_Image neighbor = *it;
                output << neighbor.GetIndex() << endl;
            }
            printProgress(static_cast<double>(query_image.GetIndex()) / query.GetImages().size());
        }
        printProgress(1.0);
        cout << endl
             << "[i] Finished Calculating Results" << endl;
        output << "===" << endl;
        output << "tAverageApproximate: " << time_aprox_sum / query.GetImages().size() << endl;
        output << "tAverageBrute: " << time_brute_sum / query.GetImages().size() << endl;
        output << "MAF: " << max_maf << endl;
        output.close();
    }
    else
    {
        cout << "DEBUG: Failed to write to output file." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}