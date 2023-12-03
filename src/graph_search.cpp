#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "argh.h"
#include "gnns.h"
#include "mrng.h"
#include "mnist.h"
#include "brute.h"
#include "misc.h"

#define K_DEFAULT 50
#define E_DEFAULT 30
#define N_DEFAULT 1
#define R_DEFAULT 1
#define l_DEFAULT 20

using namespace std;

#pragma region HELP_MESSAGE
const char *help_msg = R"""(
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;  // Input MNIST format file containing data vectors.
    string query_file;  // Query MNIST format file for nearest neighbor search.
    string output_file; // Output file to store the results.
    int no_neighbors;   // Number of LSH nearest neighbors to use (default: 40).
    int no_expansions;  // Number of expansions to use (default: 30).
    int no_nearest;     // Number of nearest points to search for (default: 1).
    int no_restarts;    // Number of random restarts (default: 1).
    int no_candidates;  // Number of candidates, only for MRNG (default: 20).
    int mode;           // Mode (1 for GNNS, 2 for MRNG).

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

    // Debug CMD arguments.
    // cout << "DEBUG: input             = " << input_file << endl;
    // cout << "DEBUG: query             = " << query_file << endl;
    // cout << "DEBUG: output            = " << output_file << endl;
    // cout << "DEBUG: no_neighbors      = " << no_neighbors << endl;
    // cout << "DEBUG: no_expansions     = " << no_expansions << endl;
    // cout << "DEBUG: no_nearest        = " << no_nearest << endl;
    // cout << "DEBUG: no_restarts       = " << no_restarts << endl;
    // cout << "DEBUG: no_candidates     = " << no_candidates << endl;
    // cout << "DEBUG: mode              = " << mode << endl;

    // In the following cases, print the help message.
    if (cmdl({"-h", "--help"}) || input_file.empty() || query_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Create the required class instances.
    MNIST input = MNIST(input_file);
    MNIST query = MNIST(query_file);
    BRUTE bf = BRUTE(input);
    set<MNIST_Image, MNIST_ImageComparator> nn;
    ofstream output(output_file, ios::out | ios::trunc);
    clock_t start, end;
    double time;
    double time_aprox_sum = 0;
    double time_brute_sum = 0;
    double max_maf = 0;

    // Print results in output file.
    if (output.is_open())
    {
        if (mode == 1)
        {
            output << "GNNS Results" << endl;
            auto gnns = GNNS(input, no_neighbors, no_expansions, no_restarts);
            gnns.Initialization();
            cout << "[i] Calculating Results" << endl;
            printProgress(0.0);
            for (auto query_image : query.GetImages())
            {
                start = clock();
                nn = gnns.FindNearestNeighbors(no_nearest, query_image);
                end = clock();
                time = double(end - start) / CLOCKS_PER_SEC;
                time_aprox_sum += time;

                output << "===" << endl;
                output << "Query: " << query_image.GetIndex() << endl;
                output << "timeGNNS: " << time << "s" << endl;

                // Print Brute
                start = clock();
                set<MNIST_Image, MNIST_ImageComparator> brute_nn = bf.FindNearestNeighbors(no_nearest, query_image);
                end = clock();
                time = double(end - start) / CLOCKS_PER_SEC;
                time_brute_sum += time;
                output << "timeBRUTE: " << time << "s" << endl;

                // Print Comparison Stats between LSH and Brute Force.
                int i = 1;
                for (auto it1 = nn.begin(), it2 = brute_nn.begin();
                     (it1 != nn.end()) && (it2 != brute_nn.end());
                     it1++, it2++)
                {
                    MNIST_Image neighbor_approx = *it1;
                    MNIST_Image neighbor_brute = *it2;

                    // calc maf
                    if (i == 1)
                    {
                        double maf = static_cast<double>(neighbor_approx.GetDist()) / static_cast<double>(neighbor_brute.GetDist());
                        if (maf > max_maf)
                            max_maf = maf;
                    }

                    output << "NN-" << i << " Index: " << neighbor_approx.GetIndex() << endl;
                    output << "distanceGNNS: " << neighbor_approx.GetDist() << endl;
                    output << "distanceMRNG: " << neighbor_brute.GetDist() << endl;
                    i++;
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
            output << "MRNG Results" << endl;
            auto mrng = MRNG(input, no_candidates);
            mrng.Initialization();
            cout << "[i] Calculating Results" << endl;
            printProgress(0.0);

            for (auto query_image : query.GetImages())
            {
                start = clock();
                nn = mrng.FindNearestNeighbors(no_nearest, query_image);
                end = clock();
                time = double(end - start) / CLOCKS_PER_SEC;
                time_aprox_sum += time;

                output << "===" << endl;
                output << "Query: " << query_image.GetIndex() << endl;
                output << "timeMRNG: " << time << "s" << endl;

                // Print Brute
                start = clock();
                set<MNIST_Image, MNIST_ImageComparator> brute_nn = bf.FindNearestNeighbors(no_nearest, query_image);
                end = clock();
                time = double(end - start) / CLOCKS_PER_SEC;
                time_brute_sum += time;
                output << "timeBRUTE: " << time << "s" << endl;

                // Print Comparison Stats between LSH and Brute Force.
                int i = 1;
                for (auto it1 = nn.begin(), it2 = brute_nn.begin();
                     (it1 != nn.end()) && (it2 != brute_nn.end());
                     it1++, it2++)
                {
                    MNIST_Image neighbor_approx = *it1;
                    MNIST_Image neighbor_brute = *it2;

                    // calc maf
                    if (i == 1)
                    {
                        double maf = static_cast<double>(neighbor_approx.GetDist()) / static_cast<double>(neighbor_brute.GetDist());
                        if (maf > max_maf)
                            max_maf = maf;
                    }

                    output << "NN-" << i << " Index: " << neighbor_approx.GetIndex() << endl;
                    output << "distanceMRNG: " << neighbor_approx.GetDist() << endl;
                    output << "distanceBRUTE: " << neighbor_brute.GetDist() << endl;
                    i++;
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
    }
    else
    {
        cout << "Failed to write to output file." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}