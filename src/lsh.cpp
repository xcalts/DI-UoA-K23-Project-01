#include <iostream>
#include <vector>
#include <cstring>

#include "mnist.h"
#include "lsh.h"

using namespace std;

/* Constructor */
LSH::LSH(MNIST input, MNIST query, string output, int k, int L, int N, int R)
{
    output_file_path = output;
    number_of_hashing_functions = k;
    number_of_hash_tables = L;
    number_of_nearest = N;
    radius = R;
    query_images = query.GetImages();
    images = input.GetImages();
}

/* Accesssors */

/* Functions */
void LSH::FindNearestNeighbor()
{
}

void LSH::FindAllNearestNeighbors()
{
}

void LSH::RadiusSearch()
{
}

void LSH::Print()
{
    cout << "Output File Path:                        " << output_file_path << endl;
    cout << "Number of Hashing Functions:             " << to_string(number_of_hashing_functions) << endl;
    cout << "Number of Hash Tables:                   " << to_string(number_of_hash_tables) << endl;
    cout << "Number of Nearest Neighbors to be Found: " << to_string(number_of_nearest) << endl;
    cout << "Radius:                                  " << to_string(radius) << endl;
}