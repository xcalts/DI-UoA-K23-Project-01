#ifndef LSH_H
#define LSH_H

#include <array>
#include <iostream>
#include <vector>
#include <string>

class LSH 
{
private:
    std::string output_file_path;
    uint8_t number_of_hashing_functions;
    uint8_t number_of_hash_tables;
    uint8_t number_of_nearest;
    uint32_t radius;
    std::vector<std::array<uint8_t, 784>> query_images;
    std::vector<std::array<uint8_t, 784>> images; // 28*28 dimensions

public:
    /* Constructor */
    LSH(const std::string, const std::string, const uint8_t, const uint8_t, const uint8_t, const uint32_t, std::vector<std::array<uint8_t, 784>> images);

    /* Accessors */
    

    /* Methods */
    void FindNearestNeighbor();

    void FindAllNearestNeighbors();

    void RadiusSearch();

    void Print();
};

#endif // LSH_H