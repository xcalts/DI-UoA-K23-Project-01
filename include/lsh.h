#ifndef LSH_H
#define LSH_H

#include <array>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class LSH
{
private:
    string output_file_path;
    int number_of_hashing_functions;
    int number_of_hash_tables;
    int number_of_nearest;
    int radius;
    vector<array<uint8_t, 784>> query_images;
    vector<array<uint8_t, 784>> images; // 28*28 dimensions

public:
    /* Constructor */
    LSH(MNIST, MNIST, string, int, int, int, int);

    /* Accessors */

    /* Methods */
    void FindNearestNeighbor();

    void FindAllNearestNeighbors();

    void RadiusSearch();

    void Print();
};

#endif // LSH_H