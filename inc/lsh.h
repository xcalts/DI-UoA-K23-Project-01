#ifndef LSH_H
#define LSH_H

#include <stdio.h>
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
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

// LSH contains the functionality of the Locality-Sensitive Hashing algorithm.
class LSH
{
private:
    int no_hash_functions;                                        // The number of hash functions inside the "amplified" one.
    int no_hash_tables;                                           // The number of hash tables used for LSH.
    vector<MNIST_Image> images;                                   // The MNIST dataset's images converted to d-vectors.
    vector<unordered_map<uint, vector<MNIST_Image>>> hash_tables; // The LSH Hash Tables.
    vector<vector<IMAGE_DATA>> random_projections;                // These are the random vectors that are used to calculate each h(p) for each hash table.

    void printProgress(double percentage)
    {
        int val = (int)(percentage * 100);
        int lpad = (int)(percentage * PBWIDTH);
        int rpad = PBWIDTH - lpad;
        printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
        fflush(stdout);
    }

    void Initialization()
    {
        cout << "LSH started hashing the dataset." << endl;

        // For each hash table, create {number_of_hashing_functions} random projections
        random_projections = GetRandomProjections(no_hash_tables, no_hash_functions);

        for (int i = 0; i < no_hash_tables; i++)
        { // For each hash table

            for (int j = 0; j < (int)images.size(); j++)
            { // For each image in input set

                // hash_code_for_querying_trick can be used as an optimization to LSH, haven't implemented it yet
                uint hash_code_for_querying_trick = CalculateFinalHashCode(images[j].GetImageData(), random_projections[i], no_hash_functions, WINDOW);
                images[j].SetId(hash_code_for_querying_trick);

                uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16)); // Mod by n/16 to get final_hash_code, found it yields the best results for W = 400

                hash_tables[i][final_hash_code].push_back(images[j]);
            }

            printProgress(static_cast<double>(i) / no_hash_tables);
        }

        printProgress(1);

        cout << endl
             << "LSH fished hashing the dataset." << endl;
    }

public:
    LSH();

    // Create a new instance of LSH.
    LSH(MNIST _input, int _no_hash_functions, int _no_hash_tables)
    {
        no_hash_functions = _no_hash_functions;
        no_hash_tables = _no_hash_tables;
        images = _input.GetImages();
        hash_tables = vector<unordered_map<uint, vector<MNIST_Image>>>(_no_hash_tables);

        Initialization();
    }

    // Find the {no_neighbors} "Nearest Neighbors" vectors of the queried one using Locality-Sensitive Hashing algorithm.
    set<MNIST_Image, MNIST_ImageComparator> FindNearestNeighbors(int no_neighbours, MNIST_Image query_image)
    {
        set<MNIST_Image, MNIST_ImageComparator> nearest_neighbors; // Used for sorting the final vector of ANN
        double min_dist = pow(2, 32) - 5;
        for (int i = 0; i < no_hash_tables; i++)
        { // For each hash table

            // Find query_image's hash code for given table, same way as for the input set
            uint hash_code_for_querying_trick = CalculateFinalHashCode(query_image.GetImageData(), random_projections[i], no_hash_functions, WINDOW);
            query_image.SetId(hash_code_for_querying_trick);
            uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16));

            // If the query_image ends up in an empty bucket for this hash table
            if (hash_tables[i][final_hash_code].size() == 0)
                continue;

            vector<MNIST_Image> bucket_images = hash_tables[i][final_hash_code];

            // For each image found in the same bucket as query_image
            // Calculate distance for each image in the same bucket as query_image (basically the whole point of LSH)
            for (int j = 0; j < (int)bucket_images.size(); j++)
            {
                if (query_image.GetId() != bucket_images[j].GetId())
                    continue;

                double dist = EuclideanDistance(2, query_image.GetImageData(), bucket_images[j].GetImageData());
                bucket_images[j].SetDist(dist);

                // If found a better ANN than the current worst ANN
                // Remove worst ANN
                // Insert new ANN, set sorts itself
                if (dist < min_dist)
                {
                    if ((int)nearest_neighbors.size() == no_neighbours)
                        nearest_neighbors.erase(--nearest_neighbors.end());

                    nearest_neighbors.insert(bucket_images[j]);
                    set<MNIST_Image, MNIST_ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin(); // Could be replaced by nearest_neighbors.end(), didn't work for some reason
                    MNIST_Image last_image = *itr;
                    min_dist = last_image.GetDist();
                }
            }
        }

        return nearest_neighbors;
    }

    // Find the vectors inside the given radius of the queried one using Locality-Sensitive Hashing algorithm.
    set<MNIST_Image, MNIST_ImageComparator> RadiusSearch(MNIST_Image query_image, int radius)
    {
        set<MNIST_Image, MNIST_ImageComparator> vectors_inside_radius; // Used for sorting the final vector of ANN

        for (int i = 0; i < no_hash_tables; i++)
        {
            // Find the queried image's hash code for the corresponding hash table.
            uint hash_code_for_querying_trick = CalculateFinalHashCode(query_image.GetImageData(), random_projections[i], no_hash_functions, WINDOW);
            uint final_hash_code = hash_code_for_querying_trick % ((uint)(images.size() / 16));

            // If the queried image ends up in an empty bucket for this hash table, then continue to the next hash table.
            if (hash_tables[i][final_hash_code].size() == 0)
                continue;

            // Else, for each image found in the same bucket as queried one,
            // calculate the distance for each image in the same bucket as the queried one.
            // If the image is inside the radius,
            // then insert it to the found vectors.
            vector<MNIST_Image> bucket_images = hash_tables[i][final_hash_code];
            for (int j = 0; j < (int)bucket_images.size(); j++)
            {
                double dist = EuclideanDistance(2, query_image.GetImageData(), bucket_images[j].GetImageData());
                bucket_images[j].SetDist(dist);

                if (dist < radius)
                {
                    vectors_inside_radius.insert(bucket_images[j]); // It sorts itself! Output will be tidy!
                }
            }
        }

        return vectors_inside_radius;
    }
};

#endif // LSH_H