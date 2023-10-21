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
    
    default_random_engine generator(static_cast<unsigned>(time(nullptr)));                         // code for generating random numbers using normal distribution
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

uint CalculateFinalHashCode(array<uint8_t, 784> image, vector<array<double, 784>> random_projections, int number_of_hashing_functions, int window)
{   
    vector<uint> hashing_functions_output(number_of_hashing_functions);
            
    for(int k = 0; k < number_of_hashing_functions; k++) {      // For each hashing function
        hashing_functions_output[k] = CalculateHashCode(image, random_projections[k], window);
    }

    uint sum = 0;

    random_device rd;
	mt19937 gen(rd());

    uniform_int_distribution<int> random_ri(-40, 40); 

    for(int k = 0; k < (int) hashing_functions_output.size(); k++) {
        int ri = random_ri(gen);
        // printf("Ri: %d\n", ri);
        sum += (ri * (uint) hashing_functions_output[k]) % 4294967291;
    }

    //might need to readress the logic here
    // printf("%ld\n", sum % 4294967291);

    // we may have to use this
    // res = a - ((floor( (long double)a / (long double)b) ) * b)
    
    uint final_hash_code = (uint) (sum % 4294967291);   //we use 4294967291 according to theory
    
    // printf("Hash Code: %d\n", final_hash_code);
    
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
    sum += rand() % (uint) window;

    double hash_code = sum / (uint) window;

    // printf("Hash Code: %f %d\n", floor(hash_code), (uint) floor(hash_code));

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