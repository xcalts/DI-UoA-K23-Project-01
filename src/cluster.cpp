#include <cstdlib>
#include <string>
#include <iostream>

#include "argh.h"

using namespace std;

#pragma region HelpMessage
const char *help_msg = R"""(
cluster - MNIST Image Clustering Tool

Usage: cluster [options] -i <input_file> -o <output_file> -c <configuration_file>

Description:
    The cluster tool is used to perform clustering on a dataset of MNIST images.
    It provides options for configuring the clustering process, including the
    assignment method and a configuration file.

Options:
    -h, --help
        Display this help message and exit.

    -m, --method lloyd
        Choose the assignment method for k-Means clustering. Options:
        - lloyd: Use Lloyd's assignment algorithm (default).
        - reverse: Use Reverse Search assignment algorithm.
        - lsh: Use LSH (Locality-Sensitive Hashing) assignment algorithm.
        - hypercube: Use the Hypercube assignment algorithm.

    -c, --configuration <configuration_file>
        Path to a configuration file.

Positional Arguments:
    -i, --input <input_file>
        Path to the MNIST dataset file.

    -o, --output <output_file>
        Path to the output file where clustering results will be saved.

Example Usage:
    cluster -m lloyd -i <input_file> -o <output_file> -c cluster.conf
    cluster -m reverse -i <input_file> -o <output_file> -c cluster.conf
    cluster -m lsh -i <input_file> -o <output_file> -c cluster.conf
    cluster -m hypercube -i <input_file> -o <output_file> -c cluster.conf

Note:
    - The MNIST dataset file should contain the MNIST images.
    - The tool will perform k-Means clustering on the MNIST dataset based on the
      provided configuration and save the clustered images in the specified output file.

Configuration File Format:
number_of_clusters: 4
number_of_vector_hash_tables: 3
number_of_vector_hash_functions: 4
max_number_M_hybercube: 10
number_of_hypercube_dimensions: 3
number_of_probes: 2

For more information, please refer to the documentation.
)""";
#pragma endregion

int main(int argc, char *argv[])
{
    string input_file;
    string output_file;
    string conf_file;
    string method;
    string completion;

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-c", "--configuration "}) >> conf_file;

    if (cmdl({"-h", "--help"}) || input_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    // Load and process data
    // Initialize centroids with k-Means++
    // Perform MacQueen K-Means clustering
    // Output results

    return EXIT_SUCCESS;
}