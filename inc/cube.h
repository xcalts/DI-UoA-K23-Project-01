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
using Vertex = vector<Image>;

class Hypercube
{
private:
    /* Fields */
    string output_file_path;
    int dimension;
    int max_candidates;
    int probes;
    int number_of_nearest;
    int radius;
    vector<Image> query_images;
    vector<Image> images;
    unordered_map<string, Vertex> vertices;             // The vertices essentially act as a Hash Table if you think about it
    vector<array<double, 784>> random_projections;      // These are the random vectors that are used to calculate each h(p) for each hash table

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

public:
    /* Constructor */
    Hypercube(MNIST input, MNIST query, string output, int d, int M, int p, int N, int R)
    {
        output_file_path = output;
        dimension = d;
        max_candidates = M;
        probes = p;
        number_of_nearest = N;
        radius = R;
        query_images = query.GetImages();
        images = input.GetImages();

    }

    /* Functions */
    void AssignInputToVertices()
    {
        cout << "Assigning the input set to hypercube's vertices..." << endl;

        // Create random projection vectors
        random_projections = GetRandomProjections(1, dimension)[0];

        for (int i = 0; i < (int)images.size(); i++)
        {
            Image image = images[i];
            string query_vertex_code = "";

            for (int j = 0; j < dimension; j++)
            { // For each dimension

                // Map the image to a binary digit, eventually creating a string of 0s and 1s
                uint hash_code = CalculateHashCode(image.GetImageData(), random_projections[j], WINDOW);

                int binary_digit = hash_code % 2;
                query_vertex_code += to_string(binary_digit);
            }

            if(vertices.find(query_vertex_code) == vertices.end()) {
                vertices[query_vertex_code] = vector<Image>(0);
            }
            vertices[query_vertex_code].push_back(image);
        }
    }

    vector<Image> GetNearestNeighborsCandidates(vector<vector<Vertex>> vertices_by_hamming_distance)
    {
        vector<Image> nearest_neighbors_candidates(0);

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

    set<Image, ImageComparator> FindAproximateNearestNeighbors(Image query_image)
    {
        cout << "Searching for " << number_of_nearest << " Nearest Neighbors using Hypercube..." << endl;

        // Find corresponding vertex for query_image
        string query_vertex_code = "";

        for (int i = 0; i < dimension; i++)
        { // For each dimension

            // Map the image to a binary digit, eventually creating a string of 0s and 1s
            uint hash_code = CalculateHashCode(query_image.GetImageData(), random_projections[i], WINDOW);

            int binary_digit = hash_code % 2;
            query_vertex_code += to_string(binary_digit);
        }

        vector<Image> query_image_vertex(0);
        if(vertices.find(query_vertex_code) != vertices.end()) {
            vector<Image> query_image_vertex = vertices[query_vertex_code];
        }

        // Organize vertices by hamming distance to the vertex corresponding to the query_image, so probing logic can take place
        vector<vector<Vertex>> vertices_by_hamming_distance(dimension + 1);

        for (int i = 0; i < (int)vertices.size(); i++)
        {
            string vertex_code = IntToBinaryString(i, dimension);
            int hamming_distance = HammingDistance(query_vertex_code, vertex_code);
            if(vertices.find(vertex_code) != vertices.end()) {
                vertices_by_hamming_distance[hamming_distance].push_back(vertices[vertex_code]);
            }
        }

        vector<Image> nearest_neighbors_candidates = GetNearestNeighborsCandidates(vertices_by_hamming_distance);

        // Compare distances to query_image
        set<Image, ImageComparator> nearest_neighbors;
        double min_dist = pow(2, 32) - 5;

        for (int i = 0; i < (int)nearest_neighbors_candidates.size(); i++)
        {
            double dist = CalculateDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < min_dist)
            {
                if ((int)nearest_neighbors.size() == number_of_nearest)
                {
                    nearest_neighbors.erase(--nearest_neighbors.end());
                }

                nearest_neighbors.insert(images[i]);

                set<Image, ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin();
                Image last_image = *itr;
                min_dist = last_image.GetDist();
            }
        }

        return nearest_neighbors;
    }

    set<Image, ImageComparator> BruteForceNearestNeighbors(Image query_image)
    {
        cout << "Searching for " << number_of_nearest << " Nearest Neighbors using Brute Force..." << endl;

        set<Image, ImageComparator> nearest_neighbors;
        double min_dist = pow(2, 32) - 5;

        for (int i = 0; i < (int)images.size(); i++)
        {
            double dist = CalculateDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < min_dist)
            {
                if ((int)nearest_neighbors.size() == number_of_nearest)
                {
                    nearest_neighbors.erase(--nearest_neighbors.end());
                }

                nearest_neighbors.insert(images[i]);

                set<Image, ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin();
                Image last_image = *itr;
                min_dist = last_image.GetDist();
            }
        }

        return nearest_neighbors;
    }

    set<Image, ImageComparator> RadiusSearch(Image query_image)
    {
        cout << "Searching for Neighbors in Radius using Hypercube..." << endl;

        // Find corresponding vertex for query_image
        string query_vertex_code = "";

        for (int i = 0; i < dimension; i++)
        {
            uint hash_code = CalculateHashCode(query_image.GetImageData(), random_projections[i], WINDOW);

            int binary_digit = hash_code % 2;
            query_vertex_code += to_string(binary_digit);
        }

        vector<Image> query_image_vertex = vertices[query_vertex_code];

        // Organize vertices by hamming distance to the vertex corresponding to the query_image
        vector<vector<Vertex>> vertices_by_hamming_distance(dimension + 1);

        for (int i = 0; i < (int)vertices.size(); i++)
        {
            string vertex_code = IntToBinaryString(i, dimension);
            int hamming_distance = HammingDistance(query_vertex_code, vertex_code);
            vertices_by_hamming_distance[hamming_distance].push_back(vertices[vertex_code]);
        }

        vector<Image> nearest_neighbors_candidates = GetNearestNeighborsCandidates(vertices_by_hamming_distance);

        // Compare distances to query_image
        set<Image, ImageComparator> nearest_neighbors;

        for (int i = 0; i < (int)nearest_neighbors_candidates.size(); i++)
        {
            double dist = CalculateDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < radius)
            {
                nearest_neighbors.insert(images[i]);
            }
        }

        return nearest_neighbors;
    }

    void Execute()
    {
        AssignInputToVertices();

        int i = 4;

        // for(int i = 0; i < (int) query_images.size(); i++) {     // For each image in the query set

        const clock_t hypercube_begin = clock();
        set<Image, ImageComparator> nearest_neighbors_hypercube = FindAproximateNearestNeighbors(query_images[i]);
        const clock_t hypercube_end = clock();

        double hypercube_time = double(hypercube_end - hypercube_begin) / CLOCKS_PER_SEC;

        cout << "Time for Hypercube: " << hypercube_time << endl;

        // Used for debugging

        for (set<Image>::iterator it = nearest_neighbors_hypercube.begin(); it != nearest_neighbors_hypercube.end(); ++it) {
            Image element = *it;

            element.Print();
            cout << element.GetDist() << endl;
        }

        const clock_t brute_begin = clock();
        set<Image, ImageComparator> nearest_neighbors_brute_force = BruteForceNearestNeighbors(query_images[i]);
        const clock_t brute_end = clock();

        double brute_time = double(brute_end - brute_begin) / CLOCKS_PER_SEC;

        cout << "Time for Brute Force: " << brute_time << endl;

        // Used for debugging

        for (set<Image>::iterator it = nearest_neighbors_brute_force.begin(); it != nearest_neighbors_brute_force.end(); ++it) {
            Image element = *it;

            element.Print();
            cout << element.GetDist() << endl;
        }

        set<Image, ImageComparator> neighbors_in_radius = RadiusSearch(query_images[i]);

        cout << "Neihbours in Radius " << radius << ": " << neighbors_in_radius.size() << endl;

        // Used for debugging

        //query_images[4].Print();

        WriteToFile(query_images[i], nearest_neighbors_hypercube, nearest_neighbors_brute_force, neighbors_in_radius, hypercube_time, brute_time);
        // }
    }

    void Print()
    {
        cout << "Output File Path:                        " << output_file_path << endl;
        cout << "Dimension:                               " << to_string(dimension) << endl;
        cout << "Max Amount of Candidates:                " << to_string(max_candidates) << endl;
        cout << "Max Amount of Probes:                    " << to_string(probes) << endl;
        cout << "Number of Nearest Neighbors to be Found: " << to_string(number_of_nearest) << endl;
        cout << "Radius:                                  " << to_string(radius) << endl;
    }

    void WriteToFile(Image query_image, set<Image, ImageComparator> nearest_neighbors_hypercube, set<Image, ImageComparator> nearest_neighbors_brute_force, set<Image, ImageComparator> neighbors_in_radius, double hypercube_time, double brute_time)
    {
        ofstream output(output_file_path, ios::app);

        if (output.is_open())
        {

            output << "Query: " << query_image.GetIndex() << endl;

            // write data for Hypercube and Brute Force
            int i = 1;
            for (set<Image, ImageComparator>::iterator it1 = nearest_neighbors_hypercube.begin(), it2 = nearest_neighbors_brute_force.begin();
                 (it1 != nearest_neighbors_hypercube.end()) && (it2 != nearest_neighbors_brute_force.end());
                 ++it1, ++it2)
            {
                Image neighbor_hypercube = *it1;
                Image neighbor_brute = *it2;

                output << "Nearest neighbor-" << i << ": " << neighbor_hypercube.GetIndex() << endl;
                output << "distanceHypercube: " << neighbor_hypercube.GetDist() << endl;
                output << "distanceTrue: " << neighbor_brute.GetDist() << endl;

                i++;
            }

            output << "tHypercube: " << hypercube_time << endl;
            output << "tTrue: " << brute_time << endl;

            // write data for radius search
            output << "R-near neighbors:" << endl;
            for (set<Image, ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
            {
                Image neighbor = *it;
                output << neighbor.GetIndex() << endl;
            }

            output.close();
        }
        else
        {
            cout << "Failed to write to output file." << endl;
        }
    }
};

#endif // CUBE_H