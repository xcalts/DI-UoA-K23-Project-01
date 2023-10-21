#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <unordered_map>
#include <set>
#include <cstdio>
#include <fstream>
#include <string>
#include <bitset>

#include "image.h"

using namespace std;
using Vertex = vector<Image>;

class Hypercube {
private:
    string output_file_path;
    int dimension;
    int max_candidates;
    int probes;
    int number_of_nearest;
    int radius; 
    vector<Image> query_images;
    vector<Image> images;
    unordered_map<string, Vertex> vertices;
    vector<array<double, 784>> random_projections;
public:
    Hypercube(MNIST, MNIST, string, int, int, int, int, int);

    /* Methods */
    void Execute();

    void AssignInputToVertices();

    vector<Image> GetNearestNeighborsCandidates(vector<vector<Vertex>>);

    set<Image, ImageComparator> FindAproximateNearestNeighbors(Image);

    set<Image, ImageComparator> BruteForceNearestNeighbors(Image);

    set<Image, ImageComparator> RadiusSearch(Image);

    void Print();

    void WriteToFile(Image, set<Image, ImageComparator>, set<Image, ImageComparator>, set<Image, ImageComparator>, double, double);
};

#endif // CUBE_H