#ifndef HASH_H
#define HASH_H

#include <cmath>
#include <array>
#include <cstdint>
#include <ctime>
#include <random>

#include "mnist.h"

using namespace std;

// Generate the random projection vectors.
vector<vector<IMAGE_DATA>> GetRandomProjections(int no_hash_tables, int no_hash_functions)
{
    vector<vector<IMAGE_DATA>> random_projections(no_hash_tables);

    default_random_engine generator(static_cast<unsigned>(time(nullptr))); // code for generating random numbers using normal distribution
    normal_distribution<double> distribution(0.0, 1.0);                    // the numbers range from -1 to +1

    for (int i = 0; i < no_hash_tables; i++)
    { // For each hash table, get an array of random projections to use
        random_projections[i] = vector<IMAGE_DATA>(no_hash_functions);
        for (int j = 0; j < no_hash_functions; j++)
        { // Get a different random vector/projection for each hash function
            for (int k = 0; k < 784; k++)
            {
                random_projections[i][j][k] = distribution(generator) + 1; // + 1 is for normalization purposes, having negative values would mess up the final hash code
            }
        }
    }

    return random_projections;
}

// This is the hash code for each different h(p) function, as shown in theory
// It's a separate function from CalculateFinalHashCode, as it is needed by it's own for Hypercube
uint CalculateHashCode(IMAGE_DATA image, IMAGE_DATA random_projection, int window)
{
    // Calculate inner product between image and random projection, aka p [dot product] v
    double sum = 0;
    for (int i = 0; i < 784; i++)
    {
        sum += (double)image[i] * random_projection[i];
    }

    srand(time(0));
    sum += rand() % (uint)window; // rand() % (uint) window === t  (theory)

    double hash_code = sum / (uint)window; // Divide by w

    return (uint)floor(hash_code);
}

// This is the final hash code barring the (% TableSize) operation at the end, so that optimization of LSH can be possible (see theory)
uint CalculateFinalHashCode(IMAGE_DATA image, vector<IMAGE_DATA> random_projections, int no_hash_functions, int window)
{
    uint sum = 0;

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> random_ri(-40, 40);

    for (int k = 0; k < no_hash_functions; k++)
    { // For each hashing function
        uint hash_code = CalculateHashCode(image, random_projections[k], window);
        int ri = random_ri(gen);

        sum += (ri * hash_code) % 4294967291; // Recall (ab) mod m = ((a mod m)(b mod m)) mod m
    }

    uint final_hash_code = (uint)(sum % 4294967291); // we use 2^32 - 5 == 4294967291 according to theory

    return final_hash_code;
}

// This function calculates the distance between 2 images depending on p, aka the metric specified (as asked)
double EuclideanDistance(int p, IMAGE_DATA data_point_a, IMAGE_DATA data_point_b)
{
    double sum = 0.0;
    for (size_t i = 0; i < 784; i++)
    {
        double diff = data_point_a[i] - data_point_b[i];
        sum += pow(abs(diff), p);
    }

    return pow(sum, 1.0 / p);
}

#endif // HASH_H