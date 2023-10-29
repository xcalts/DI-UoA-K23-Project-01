#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <unordered_map>
#include <set>
#include <cstdio>
#include <fstream>
#include <string>
#include <bitset>

#include "hash.h"
#include "mnist.h"

#define WINDOW 400

using namespace std;
using Vertex = vector<MNIST_Image>;

// Hypercube contains the functionality of the Hypercube algorithm.
class Hypercube
{
private:
    /* Fields */
    int dimension;
    int max_candidates;
    int probes;
    vector<MNIST_Image> images;
    unordered_map<string, Vertex> vertices; // The vertices essentially act as a Hash Table if you think about it
    vector<IMAGE_DATA> random_projections;  // These are the random vectors that are used to calculate each h(p) for each hash table

    /* Functions */
    string IntToBinaryString(int num, int num_bits)
    {
        return bitset<32>(num).to_string().substr(32 - num_bits);
    }

    int HammingDistance(const std::string &str1, const std::string &str2)
    {
        // Check if the strings have the same length
        if (str1.length() != str2.length())
        {
            throw std::invalid_argument("Input strings must have the same length.");
        }

        int distance = 0;

        for (size_t i = 0; i < str1.length(); i++)
        {
            if (str1[i] != str2[i])
            {
                distance++;
            }
        }

        return distance;
    }

    void Initialization()
    {
        cout << "DEBUG: Assigning the input set to hypercube's vertices." << endl;

        // Create random projection vectors
        random_projections = GetRandomProjections(1, dimension)[0];

        for (int i = 0; i < (int)images.size(); i++)
        {
            MNIST_Image image = images[i];
            string query_vertex_code = "";

            for (int j = 0; j < dimension; j++)
            { // For each dimension

                // Map the image to a binary digit, eventually creating a string of 0s and 1s
                uint hash_code = CalculateHashCode(image.GetImageData(), random_projections[j], WINDOW);

                int binary_digit = hash_code % 2;
                query_vertex_code += to_string(binary_digit);
            }

            if (vertices.find(query_vertex_code) == vertices.end())
            {
                vertices[query_vertex_code] = vector<MNIST_Image>(0);
            }
            vertices[query_vertex_code].push_back(image);
        }
    }

    vector<MNIST_Image> GetNearestNeighborsCandidates(vector<vector<Vertex>> vertices_by_hamming_distance)
    {
        vector<MNIST_Image> nearest_neighbors_candidates(0);

        for (int i = 0; i < probes; i++)
        {

            for (int j = 0; j < (int)vertices_by_hamming_distance[i].size(); j++)
            { // For every vertex with 0 <= hamming distance to query_vertex <= probes (opbv probes <= dimension)
                Vertex vertex = vertices_by_hamming_distance[i][j];

                for (int k = 0; k < (int)vertex.size(); k++)
                { // For every image of said vertex

                    // If we've reached max candidates then return
                    if ((int)nearest_neighbors_candidates.size() == max_candidates)
                    {
                        return nearest_neighbors_candidates;
                    }

                    nearest_neighbors_candidates.push_back(vertex[k]);
                }
            }
        }

        return nearest_neighbors_candidates;
    }

public:
    // Create a new instance of LSH.
    Hypercube(MNIST _input, int _d, int _M, int _p)
    {
        dimension = _d;
        max_candidates = _M;
        probes = _p;
        images = _input.GetImages();

        Initialization();
    }

    // Find the {no_nearest} "Nearest Neighbors" vectors of the queried one using the Hypercube algorithm.
    set<MNIST_Image, MNIST_ImageComparator> FindNearestNeighbors(int no_neighbours, MNIST_Image query_image)
    {
        cout << "DEBUG: Searching for " << no_neighbours << " Nearest Neighbors using Hypercube." << endl;

        // Find corresponding vertex for query_image
        string query_vertex_code = "";

        for (int i = 0; i < dimension; i++)
        { // For each dimension

            // Map the image to a binary digit, eventually creating a string of 0s and 1s
            uint hash_code = CalculateHashCode(query_image.GetImageData(), random_projections[i], WINDOW);

            int binary_digit = hash_code % 2;
            query_vertex_code += to_string(binary_digit);
        }

        vector<MNIST_Image> query_image_vertex(0);
        if (vertices.find(query_vertex_code) != vertices.end())
        {
            vector<MNIST_Image> query_image_vertex = vertices[query_vertex_code];
        }

        // Organize vertices by hamming distance to the vertex corresponding to the query_image, so probing logic can take place
        vector<vector<Vertex>> vertices_by_hamming_distance(dimension + 1);

        for (int i = 0; i < (int)vertices.size(); i++)
        {
            string vertex_code = IntToBinaryString(i, dimension);
            int hamming_distance = HammingDistance(query_vertex_code, vertex_code);
            if (vertices.find(vertex_code) != vertices.end())
            {
                vertices_by_hamming_distance[hamming_distance].push_back(vertices[vertex_code]);
            }
        }

        vector<MNIST_Image> nearest_neighbors_candidates = GetNearestNeighborsCandidates(vertices_by_hamming_distance);

        // Compare distances to query_image
        set<MNIST_Image, MNIST_ImageComparator> nearest_neighbors;
        double min_dist = pow(2, 32) - 5;

        for (int i = 0; i < (int)nearest_neighbors_candidates.size(); i++)
        {
            double dist = EuclideanDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < min_dist)
            {
                if ((int)nearest_neighbors.size() == no_neighbours)
                {
                    nearest_neighbors.erase(--nearest_neighbors.end());
                }

                nearest_neighbors.insert(images[i]);

                set<MNIST_Image, MNIST_ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin();
                MNIST_Image last_image = *itr;
                min_dist = last_image.GetDist();
            }
        }

        return nearest_neighbors;
    }

    // Find the vectors inside the given radius of the queried one using Hypercube algorithm.
    set<MNIST_Image, MNIST_ImageComparator> RadiusSearch(MNIST_Image query_image, int radius)
    {
        cout << "DEBUG: Searching for Neighbors in Radius using Hypercube." << endl;

        // Find corresponding vertex for query_image
        string query_vertex_code = "";

        for (int i = 0; i < dimension; i++)
        {
            uint hash_code = CalculateHashCode(query_image.GetImageData(), random_projections[i], WINDOW);

            int binary_digit = hash_code % 2;
            query_vertex_code += to_string(binary_digit);
        }

        vector<MNIST_Image> query_image_vertex = vertices[query_vertex_code];

        // Organize vertices by hamming distance to the vertex corresponding to the query_image
        vector<vector<Vertex>> vertices_by_hamming_distance(dimension + 1);

        for (int i = 0; i < (int)vertices.size(); i++)
        {
            string vertex_code = IntToBinaryString(i, dimension);
            int hamming_distance = HammingDistance(query_vertex_code, vertex_code);
            vertices_by_hamming_distance[hamming_distance].push_back(vertices[vertex_code]);
        }

        vector<MNIST_Image> nearest_neighbors_candidates = GetNearestNeighborsCandidates(vertices_by_hamming_distance);

        // Compare distances to query_image
        set<MNIST_Image, MNIST_ImageComparator> nearest_neighbors;

        for (int i = 0; i < (int)nearest_neighbors_candidates.size(); i++)
        {
            double dist = EuclideanDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < radius)
            {
                nearest_neighbors.insert(images[i]);
            }
        }

        return nearest_neighbors;
    }
};

#endif // CUBE_H