#include <fstream>
#include <string>
#include <vector>

#include "argh.h"
#include "brute.h"
#include "lsh.h"
#include "mnist.h"
#include "mrng.h"

#define K_DEFAULT 4
#define L_DEFAULT 5
#define N_DEFAULT 1
#define R_DEFAULT 10000

using namespace std;

#pragma region HELP_MESSAGE
const char *help_msg = R"""(
MRNG Graph Algorithm for Vectors in d-Space


)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;  // Input MNIST format file containing data vectors.
    string query_file;  // Query MNIST format file for nearest neighbor search.
    string output_file; // Output file to store the results.
    int no_candinates;  // Number of candinates.
    int no_nearest;     // Number of nearest points to search for (default: 1).

    // Parse the command line arguments using the argh.h functionality.
    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-q", "--query"}) >> query_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-I", "--num-candinates"}, N_DEFAULT) >> no_candinates;
    cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;

    // In the following cases, print the help message.
    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Create the required class instances.
    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);
    // MRNG mrng = MRNG(input, no_candinates);
    LSH lsh = LSH(input, 3, 4);
    BRUTE bf = BRUTE(input);
    ofstream output(output_file, ios::out | ios::trunc);
    clock_t start, end;
    double time;

    // Print results in output file.
    if (output.is_open())
    {
        for (MNIST_Image query_image : query.GetImages())
        {
            output << "====================================================================================" << endl;
            output << "Query: " << query_image.GetIndex() << endl;

            // First print the image to have an idea what we are looking for.
            output << query_image.Print() << endl;

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

                output << neighbor_lsh.Print() << endl;

                output << "NN-" << i << " Index: " << neighbor_lsh.GetIndex() << endl;
                output << "distanceLSH: " << neighbor_lsh.GetDist() << endl;
                output << "distanceBRUTE: " << neighbor_brute.GetDist() << endl;
                i++;
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