#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <ctime>

#include "mnist.h"
#include "hashing.h"
#include "lsh.h"

using namespace std;

#define WINDOW 400

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

    hash_tables = vector<unordered_map<uint, vector<array<uint8_t, 784>>>>(L);
}

/* Accesssors */

/* Functions */
void LSH::GenerateHashFunctions()
{
    // generate k hash functions for each hash tables, using random projection vectors
    
    // create k projection vectors, for each table. there are L tables. each projection vector has 784 doubles in it
    
    // empty for now, might be needed
}

void LSH::HashInput()
{
    // for each hash table, use it's hashing functions and hash the input dataset

    // create random projection vectors
    random_projections = GetRandomProjections(number_of_hash_tables, number_of_hashing_functions);

    for(int i = 0; i < number_of_hash_tables; i++) {   // For each hash table

        for(int j = 0; j < (int) images.size(); j++) {       // For each image in input set
            uint final_hash_code = CalculateFinalHashCode(images[j], random_projections[i], number_of_hashing_functions, images.size(), WINDOW);
            hash_tables[i][final_hash_code].push_back(images[j]);
        }
    }
}

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