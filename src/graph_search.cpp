#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "argh.h"
#include "gnns.h"
#include "mrng.h"
#include "mnist.h"

#define K_DEFAULT 50
#define E_DEFAULT 30
#define N_DEFAULT 1
#define R_DEFAULT 1
#define l_DEFAULT 20

using namespace std;

#pragma region HELP_MESSAGE
const char *help_msg = R"""(
GNNS Algorithm for Vectors in d-Space

Usage:
lsh [options]

Options:
-h, --help                   Print the help message.
-i, --input <input_file>     Input MNIST format file containing data vectors.
-q, --query <query_file>     Query MNIST format file for graph search.
-o, --output <output_file>   Output file to store the results.
-k, --num-neighbors <k>      Number of LSH nearest neighbors to use (default: 50).
-E, --num-expansions <E>     Number of expansions (default: 30).
-N, --num-nearest <N>        Number of nearest points to search for (default: 1).
-R, --num-restarts <R>       Number of random restarts (default: 1).
-l, --num-candidates <l>     Number of candidates, only for MRNG (default: 20).
-m, --mode <m>               Mode (1 for GNNS, 2 for MRNG).

Description:
This command line tool implements the Graph Search algorithm for vectors in d-space.

Example Usage:
graph_search -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -k 15 -E 10 -N 5 -R 5000 -m 1

Note:
The input and query files should be in the MNIST format.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;     // Input MNIST format file containing data vectors.
    string query_file;     // Query MNIST format file for nearest neighbor search.
    string output_file;    // Output file to store the results.
    int no_neighbors;      // Number of LSH nearest neighbors to use (default: 40).
    int no_expansions;     // Number of expansions to use (default: 30).
    int no_nearest;        // Number of nearest points to search for (default: 1).
    int no_restarts;       // Number of random restarts (default: 1).
    int no_candidates;     // Number of candidates, only for MRNG (default: 20).
    int mode;              // Mode (1 for GNNS, 2 for MRNG).

    // Parse the command line arguments using the argh.h functionality.
    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-k", "--num-neighbors"}, K_DEFAULT) >> no_neighbors;
    cmdl({"-E", "--num-expansions"}, E_DEFAULT) >> no_expansions;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;
    cmdl({"-R", "--num-restarts"}, R_DEFAULT) >> no_restarts;
    cmdl({"-l", "--num-candidates"}, l_DEFAULT) >> no_candidates;
    cmdl({"-m", "--mode"}, 1) >> mode;

    cout << "DEBUG: input             = " << input_file << endl;
    cout << "DEBUG: query             = " << query_file << endl;
    cout << "DEBUG: output            = " << output_file << endl;
    cout << "DEBUG: no_neighbors      = " << no_neighbors << endl;
    cout << "DEBUG: no_expansions     = " << no_expansions << endl;
    cout << "DEBUG: no_nearest        = " << no_nearest << endl;
    cout << "DEBUG: no_restarts       = " << no_restarts << endl;
    cout << "DEBUG: no_candidates     = " << no_candidates << endl;
    cout << "DEBUG: mode              = " << mode << endl;

    // In the following cases, print the help message.
    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Create the required class instances.
    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);
    ofstream output(output_file, ios::out | ios::trunc);
    
    GNNS gnns = GNNS(input, no_neighbors, no_expansions, no_restarts);

    if(mode == 1)
    {
        gnns.Initialization();
        // gnns.PrintGraph();
        
        cout << input.GetImages()[2].Print() << endl;

        set<MNIST_Image, MNIST_ImageComparator> nn = gnns.FindNearestNeighbors(1, input.GetImages()[2]);

        cout << nn.size() << endl;

        for (auto it = nn.begin();
             it != nn.end();
             it++)
        {
            MNIST_Image neighbor = *nn.begin();

            if (output.is_open())
            {
                output << neighbor.Print() << endl;
            }
            
        }
    } 
    else if (mode == 2)
    {

    }

    // ofstream output(output_file, ios::out | ios::trunc);
    // clock_t start, end;
    // double time;

    // Print results in output file.
    // if (output.is_open())
    // {
    //     for (MNIST_Image query_image : query.GetImages())
    //     {
    //         output << "====================================================================================" << endl;
    //         output << "Query: " << query_image.GetIndex() << endl;

    //         // First print the image to have an idea what we are looking for.
    //         output << query_image.Print() << endl;

    //         // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Locality-Sensitive Hashing.
    //         start = clock();
    //         // set<MNIST_Image, MNIST_ImageComparator> lsh_nn = lsh.FindNearestNeighbors(no_nearest, query_image);
    //         end = clock();
    //         time = double(end - start) / CLOCKS_PER_SEC;
    //         output << "timeLSH: " << time << " // seconds" << endl;

    //         // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Brute Force.
    //         start = clock();
    //         // set<MNIST_Image, MNIST_ImageComparator> brute_nn = bf.FindNearestNeighbors(no_nearest, query_image);
    //         end = clock();
    //         time = double(end - start) / CLOCKS_PER_SEC;
    //         output << "timeBRUTE: " << time << " // seconds" << endl;

    //         // Print Comparison Stats between LSH and Brute Force.
    //         int i = 1;
    //         // for (auto it1 = lsh_nn.begin(), it2 = brute_nn.begin();
    //         //      (it1 != lsh_nn.end()) && (it2 != brute_nn.end());
    //         //      it1++, it2++)
    //         // {
    //         //     MNIST_Image neighbor_lsh = *it1;
    //         //     MNIST_Image neighbor_brute = *it2;

    //         //     output << neighbor_lsh.Print() << endl;

    //         //     output << "NN-" << i << " Index: " << neighbor_lsh.GetIndex() << endl;
    //         //     output << "distanceLSH: " << neighbor_lsh.GetDist() << endl;
    //         //     output << "distanceBRUTE: " << neighbor_brute.GetDist() << endl;
    //         //     i++;
    //         // }

    //         // Find the Neighbors inside the radius.
    //         // set<MNIST_Image, MNIST_ImageComparator> neighbors_in_radius = lsh.RadiusSearch(query_image, radius);
    //         // output << "Radius: " << radius << endl;
    //         // for (auto it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
    //         // {
    //         //     MNIST_Image neighbor = *it;

    //         //     output << neighbor.GetIndex() << endl;
    //         // }
    //     }

    //     output.close();
    // }
    // else
    // {
    //     cout << "Failed to write to output file." << endl;
    //     return EXIT_FAILURE;
    // }

    return EXIT_SUCCESS;
}