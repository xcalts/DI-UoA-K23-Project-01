#include <cmath>
#include <array>
#include <cstdint>
#include <ctime>
#include <random>

#include "hashing.h"

using namespace std;

/* Helper functions */
vector<vector<array<double, 784>>> GetRandomProjections(int number_of_hash_tables, int number_of_hashing_functions)
{
    vector<vector<array<double, 784>>> random_projections(number_of_hash_tables);
    
    default_random_engine generator;                         // code for generating random numbers using normal distribution
    normal_distribution<double> distribution(0.0, 1.0);

    for(int i = 0; i < number_of_hash_tables; i++) {
        random_projections[i] = vector<array<double, 784>>(number_of_hashing_functions);
        for(int j = 0; j < number_of_hashing_functions; j++) {
            for(int k = 0; k < 784; k++) {
                random_projections[i][j][k] = distribution(generator) + 1; // + 1 is for normalization purposes
            }
        }
    }

    return random_projections;
}

uint CalculateHashCode(array<uint8_t, 784> image, array<double, 784> random_projection, int window);

uint CalculateFinalHashCode(array<uint8_t, 784> image, vector<array<double, 784>> random_projections, int number_of_hashing_functions, int n, int window)
{   
    vector<uint> hashing_functions_output(number_of_hashing_functions);
            
    for(int k = 0; k < number_of_hashing_functions; k++) {      // For each hashing function
        hashing_functions_output[k] = CalculateHashCode(image, random_projections[k], window);
    }

    uint sum = 0;

    srand(time(0));

    for(int k = 0; k < (int) hashing_functions_output.size(); k++) {
        sum += (uint) rand() * (uint) hashing_functions_output[k];
    }

    //might need to readress the logic here
    // printf("%ld\n", sum % 4294967291);
    uint final_hash_code = ((uint) (sum % 4294967291)) % ((uint) (n / 8));   //we use 4294967291 according to theory

    return final_hash_code;
}

uint CalculateHashCode(array<uint8_t, 784> image, array<double, 784> random_projection, int window)
{   
    // calculate inner product between image and random projection
    double sum = 0;
    for(int i = 0; i < 784; i++) {
        sum += (double) image[i] * random_projection[i];
    }

    srand(time(0));
    sum += rand() % window;

    double hash_code = sum / window;

    return (uint) floor(hash_code);
}

double CalculateDistance(int p, array<uint8_t, 784> data_point_a, array<uint8_t, 784> data_point_b)
{
    double sum = 0.0;
    for (size_t i = 0; i < 784; i++) {
        double diff = data_point_a[i] - data_point_b[i];
        sum += pow(abs(diff), p);
    }

    return pow(sum, 1.0 / p);
}