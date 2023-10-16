#include <iostream>
#include <vector>
#include <cstring>

#include "mnist.h"
#include "lsh.h"

#define K_DEFAULT 4
#define L_DEFAULT 5
#define N_DEFAULT 1
#define R_DEFAULT 10000

/* Constructor */
LSH::LSH(const std::string query_filepath, const std::string output_filepath, const uint8_t k, const uint8_t L, const uint8_t N, const uint32_t R, std::vector<std::array<uint8_t, 784>> images)
{
    output_file_path = output_filepath;

    if(!k)
        number_of_hashing_functions = K_DEFAULT;
    else
        number_of_hashing_functions = k;

    if(!L)
        number_of_hash_tables = L_DEFAULT;
    else
        number_of_hash_tables = L;

    if(!N)
        number_of_nearest = N_DEFAULT;
    else
        number_of_nearest = N;

    if(!R)
        radius = R_DEFAULT;
    else
        radius = R;

    images = images;

    MNIST query_mnist = MNIST(query_filepath);
    query_images = query_mnist.GetImages();

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
    std::cout << "Output File Path:                        " << output_file_path << std::endl;
    std::cout << "Number of Hashing Functions:             " << std::to_string(number_of_hashing_functions) << std::endl;
    std::cout << "Number of Hash Tables:                   " << std::to_string(number_of_hash_tables) << std::endl;
    std::cout << "Number of Nearest Neighbors to be Found: " << std::to_string(number_of_nearest) << std::endl;
    std::cout << "Radius:                                  " << std::to_string(radius) << std::endl;
}