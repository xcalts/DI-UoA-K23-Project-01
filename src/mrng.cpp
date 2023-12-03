#include <fstream>
#include <string>
#include <vector>

// #include "argh.h"
// #include "brute.h"
// #include "lsh.h"
// #include "mnist.h"
// #include "mrng.h"

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
    // string input_file;  // Input MNIST format file containing data vectors.
    // string query_file;  // Query MNIST format file for nearest neighbor search.
    // string output_file; // Output file to store the results.
    // int no_candinates;  // Number of candinates.
    // int no_nearest;     // Number of nearest points to search for (default: 1).

    // // Parse the command line arguments using the argh.h functionality.
    // argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    // cmdl({"-i", "--input"}) >> input_file;
    // cmdl({"-q", "--query"}) >> query_file;
    // cmdl({"-o", "--output"}) >> output_file;
    // cmdl({"-I", "--num-candinates"}, N_DEFAULT) >> no_candinates;
    // cmdl({"-N", "--num-nearest"}, N_DEFAULT) >> no_nearest;

    // // In the following cases, print the help message.
    // if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    // {
    //     cout << help_msg << endl;
    //     return EXIT_FAILURE;
    // }

    // // Create the required class instances.
    // cout << "[i] Initializing the MNIST dataset and MNIST query." << endl;
    // MNIST input = MNIST(input_file);
    // MNIST query = MNIST(query_file);
    // MRNG mrng = MRNG(input, no_candinates);
    // BRUTE bf = BRUTE(input);

    // clock_t start, end;
    // double time;

    // ofstream output(output_file, ios::out | ios::trunc);

    // cout << "[i] Starting to write results." << endl;
    // if (output.is_open())
    // {
    //     for (MNIST_Image query_image : query.GetImages())
    //     {
    //         // Info
    //         output << "=====" << endl;
    //         output << "[i] Query no " << query_image.GetIndex() << endl;

    //         // Time MRNG
    //         start = clock();
    //         MNIST_Image mrng_nn = mrng.FindNearestNeighbor(query_image);
    //         end = clock();
    //         time = double(end - start) / CLOCKS_PER_SEC;
    //         output << "[i] TimeMRNG: " << time << "s" << endl;
    //         output << "[i] DistMRNG: " << mrng_nn.GetDist() << endl;

    //         // Time Brute-Force
    //         start = clock();
    //         MNIST_Image brute_nn = *(bf.FindNearestNeighbors(1, query_image).begin());
    //         end = clock();
    //         time = double(end - start) / CLOCKS_PER_SEC;
    //         output << "[i] TimeBRUTE: " << time << "s" << endl;
    //         output << "[i] DistBRUTE: " << brute_nn.GetDist() << endl;
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