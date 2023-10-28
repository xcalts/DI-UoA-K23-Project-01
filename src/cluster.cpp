#include <cstdlib>
#include <string>
#include <iostream>

#define RYML_SINGLE_HDR_DEFINE_NOW

#include "argh.h"
#include "mnist.h"
#include "rapidyaml.h"

using namespace std;

#pragma region HELP_MESSAGE
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

string readFileToString(const std::string &filename)
{
    std::ifstream file(filename);
    if (file)
    {
        // Read the entire file into a string
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }
    else
    {
        // Handle the case where the file couldn't be opened
        throw std::runtime_error("Error opening the file: " + filename);
    }
}

int main(int argc, char *argv[])
{
    string input_file;
    string output_file;
    string conf_file;
    string method;
    string completion;
    int no_clusters;
    int no_hash_tables;
    int no_hash_functions;
    int no_max_hypercubes;
    int no_dim_hypercubes;
    int no_probes;

    argh::parser cmdl(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);
    cmdl({"-i", "--input"}) >> input_file;
    cmdl({"-o", "--output"}) >> output_file;
    cmdl({"-c", "--configuration"}) >> conf_file;
    cmdl({"-m", "--method"}) >> method;

    if (cmdl({"-h", "--help"}) || input_file.empty() || output_file.empty())
    {
        cout << help_msg << endl;
        return EXIT_FAILURE;
    }

    string conf_contents = readFileToString(conf_file);
    ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(conf_contents));

    clusters = atoi(tree["number_of_clusters"]);
    no_hash_tables = atoi(tree["number_of_vector_hash_tables"]);
    no_hash_functions = atoi(tree["number_of_vector_hash_functions"]);
    no_max_hypercubes = atoi(tree["max_number_M_hybercube"]);
    no_dim_hypercubes = atoi(tree["number_of_hypercube_dimensions"]);
    no_probes = atoi(tree["number_of_probes"]);

    MNIST input = MNIST(input_file);

    // Load and process data
    // Initialize centroids with k-Means++
    // Perform MacQueen K-Means clustering
    // Output results

    return EXIT_SUCCESS;
}