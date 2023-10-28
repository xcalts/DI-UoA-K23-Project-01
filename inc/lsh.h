#ifndef LSH_H
#define LSH_H

#include <array>
#include <fstream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <random>

#include "hash.h"
#include "mnist.h"

using namespace std;

#define WINDOW 400

class LSH
{
private:
    string output_file_path;
    int number_of_hashing_functions;
    int number_of_hash_tables;
    int radius;
    vector<Image> query_images;
    vector<Image> images;
    vector<unordered_map<uint, vector<Image>>> hash_tables; // Vector of {number_of_hash_tables} Hash Tables
    vector<vector<array<double, 784>>> random_projections;  // These are the random vectors that are used to calculate each h(p) for each hash table

public:
    /* Constructor */
    LSH(MNIST input, MNIST query, string output, int k, int L, int R)
    {
        output_file_path = output;
        number_of_hashing_functions = k;
        number_of_hash_tables = L;
        radius = R;
        query_images = query.GetImages();
        images = input.GetImages();

        hash_tables = vector<unordered_map<uint, vector<Image>>>(L);
        HashInput();
    }

    /* Functions */
    void HashInput()
    {
        // for each hash table, use it's hashing functions and hash the input dataset
        cout << "Hashing the input set into buckets..." << endl;

        // For each hash table, create {number_of_hashing_functions} random projections
        random_projections = GetRandomProjections(number_of_hash_tables, number_of_hashing_functions);

        for (int i = 0; i < number_of_hash_tables; i++)
        { // For each hash table

            for (int j = 0; j < (int)images.size(); j++)
            { // For each image in input set

                // hash_code_for_querying_trick can be used as an optimization to LSH, haven't implemented it yet
                uint hash_code_for_querying_trick = CalculateFinalHashCode(images[j].GetImageData(), random_projections[i], number_of_hashing_functions, WINDOW);
                images[j].SetId(hash_code_for_querying_trick);

                uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16)); // Mod by n/16 to get final_hash_code, found it yields the best results for W = 400

                hash_tables[i][final_hash_code].push_back(images[j]);
            }
        }

        cout << "Finished hashing the input set..." << endl;
    }

    set<Image, ImageComparator> FindAproximateNearestNeighbors(int no_neighbours, Image query_image)
    {
        cout << "Searching for " << no_neighbours << " Nearest Neighbors using LSH..." << endl;

        set<Image, ImageComparator> nearest_neighbors; // Used for sorting the final vector of ANN
        double min_dist = pow(2, 32) - 5;
        for (int i = 0; i < number_of_hash_tables; i++)
        { // For each hash table

            // Find query_image's hash code for given table, same way as for the input set
            uint hash_code_for_querying_trick = CalculateFinalHashCode(query_image.GetImageData(), random_projections[i], number_of_hashing_functions, WINDOW);
            query_image.SetId(hash_code_for_querying_trick);
            uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16));

            // If the query_image ends up in an empty bucket for this hash table
            if (hash_tables[i][final_hash_code].size() == 0)
                continue;

            vector<Image> bucket_images = hash_tables[i][final_hash_code];

            // For each image found in the same bucket as query_image
            // Calculate distance for each image in the same bucket as query_image (basically the whole point of LSH)
            for (int j = 0; j < (int)bucket_images.size(); j++)
            {
                if (query_image.GetId() != bucket_images[j].GetId())
                    continue;

                double dist = CalculateDistance(2, query_image.GetImageData(), bucket_images[j].GetImageData());
                bucket_images[j].SetDist(dist);

                // If found a better ANN than the current worst ANN
                // Remove worst ANN
                // Insert new ANN, set sorts itself
                if (dist < min_dist)
                {
                    if ((int)nearest_neighbors.size() == no_neighbours)
                        nearest_neighbors.erase(--nearest_neighbors.end());

                    nearest_neighbors.insert(bucket_images[j]);
                    set<Image, ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin(); // Could be replaced by nearest_neighbors.end(), didn't work for some reason
                    Image last_image = *itr;
                    min_dist = last_image.GetDist();
                }
            }
        }

        return nearest_neighbors;
    }

    set<Image, ImageComparator> BruteForceNearestNeighbors(int no_neighbours, Image query_image)
    {
        cout << "Searching for " << no_neighbours << " Nearest Neighbors using Brute Force..." << endl;

        set<Image, ImageComparator> nearest_neighbors; // Used for sorting the final vector of ANN
        double min_dist = pow(2, 32) - 5;

        for (int i = 0; i < (int)images.size(); i++)
        {
            double dist = CalculateDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < min_dist)
            {
                if ((int)nearest_neighbors.size() == no_neighbours)
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
        cout << "Searching for Neighbors in Radius using LSH..." << endl;

        set<Image, ImageComparator> nearest_neighbors; // Used for sorting the final vector of ANN

        for (int i = 0; i < number_of_hash_tables; i++)
        { // For each hash table

            // Find query_image's hash code for given table, same way as for the input set
            uint hash_code_for_querying_trick = CalculateFinalHashCode(query_image.GetImageData(), random_projections[i], number_of_hashing_functions, WINDOW);
            uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16));

            if (hash_tables[i][final_hash_code].size() == 0)
            { // If the query_image ends up in an empty bucket for this hash table
                continue;
            }

            vector<Image> bucket_images = hash_tables[i][final_hash_code];

            for (int j = 0; j < (int)bucket_images.size(); j++)
            { // For each image found in the same bucket as query_image

                // Calculate distance for each image in the same bucket as query_image (basically the whole point of LSH)
                double dist = CalculateDistance(2, query_image.GetImageData(), bucket_images[j].GetImageData());
                bucket_images[j].SetDist(dist);

                if (dist < radius)
                {
                    nearest_neighbors.insert(bucket_images[j]); // nearest_neighbors sorts itself, output.txt comes out tidy!
                }
            }
        }

        return nearest_neighbors;
    }

    void Print()
    {
        cout << "Output File Path:                        " << output_file_path << endl;
        cout << "Number of Hashing Functions:             " << to_string(number_of_hashing_functions) << endl;
        cout << "Number of Hash Tables:                   " << to_string(number_of_hash_tables) << endl;
        cout << "Radius:                                  " << to_string(radius) << endl;
    }
};

#endif // LSH_H