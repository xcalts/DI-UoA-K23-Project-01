#ifndef LSH_H
#define LSH_H

#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>

#include "image.h"

using namespace std;

class LSH
{
private:
    string output_file_path;
    int number_of_hashing_functions;
    int number_of_hash_tables;
    int number_of_nearest;
    int radius;
    vector<Image> query_images;
    vector<Image> images; // 28*28 dimensions
    vector<unordered_map<uint, vector<Image>>> hash_tables;
    vector<vector<array<double, 784>>> random_projections;

public:
    /* Constructor */
    LSH(MNIST, MNIST, string, int, int, int, int);

    /* Accessors */


    /* Methods */
    void Execute();

    void HashInput();

    set<Image, ImageComparator> FindAllNearestNeighbors(Image);

    set<Image, ImageComparator> BruteForceNearestNeighbors(Image);

    set<Image, ImageComparator> RadiusSearch(Image);

    void Print();

    void WriteToFile(Image, set<Image, ImageComparator>, set<Image, ImageComparator>, set<Image, ImageComparator>, double, double);
};

#endif // LSH_H