#ifndef CLUSTER_H
#define CLUSTER_H

#include <array>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <limits>

#include "mnist.h"

using namespace std;

class Cluster
{
private:
    /* Fields */
    int no_clusters;
    int no_hash_tables;
    int no_hash_functions;
    int no_max_hypercubes;
    int no_dim_hypercubes;
    int no_probes;
    vector<Image> image_dataset;
    vector<array<uint8_t, 784>> cluster_centers;
    vector<int> assignments;

    // Function to calculate the Euclidean distance between two data points
    double euclideanDistance(const array<uint8_t, 784> &a, const array<uint8_t, 784> &b)
    {
        double sum = 0.0;
        for (int i = 0; i < 784; i++)
        {
            sum += std::pow(a[i] - b[i], 2);
        }
        return sqrt(sum);
    }

public:
    /* Constructors*/
    Cluster(int _no_clusters,
            int _no_hash_tables,
            int _no_hash_functions,
            int _no_max_hypercubes,
            int _no_dim_hypercubes,
            int _no_probes,
            vector<Image> _image_dataset)
    {
        no_clusters = _no_clusters;
        no_hash_tables = _no_hash_tables;
        no_hash_functions = _no_hash_functions;
        no_max_hypercubes = _no_max_hypercubes;
        no_dim_hypercubes = _no_dim_hypercubes;
        no_probes = _no_probes;
        image_dataset = _image_dataset;
        assignments = vector<int>(image_dataset.size());

        Initialization();
    }

    /* Initialization k-means++ */
    void Initialization()
    {
        cluster_centers = initializeClusterCenters();
        assignments = assignToNearestCluster();
    }

    // Function to initialize cluster centers using k-Means++
    vector<array<uint8_t, 784>> initializeClusterCenters()
    {
        vector<array<uint8_t, 784>> centers;
        centers.push_back(image_dataset[std::rand() % image_dataset.size()].GetImageData());

        while (centers.size() < no_clusters)
        {
            // Calculate the minimum distance from each data point to the nearest center
            std::vector<double> distances(image_dataset.size(), std::numeric_limits<double>::max());
            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                for (const array<uint8_t, 784> &center : centers)
                {
                    double dist = euclideanDistance(image_dataset[i].GetImageData(), center);
                    distances[i] = std::min(distances[i], dist);
                }
            }

            // Calculate the total distance from each data point to its nearest center
            double totalDistance = 0.0;
            for (double dist : distances)
            {
                totalDistance += dist;
            }

            // Choose the next center based on the probability proportional to its distance
            double randValue = (std::rand() / (double)RAND_MAX) * totalDistance;
            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                randValue -= distances[i];
                if (randValue <= 0.0)
                {
                    centers.push_back(image_dataset[i].GetImageData()); // Add the data point as a new center
                    break;
                }
            }
        }

        return centers; // Return the initialized cluster centers
    }

    // Function to assign each data point to the nearest cluster center using Lloyd's algorithm
    vector<int> assignToNearestCluster()
    {
        vector<int> assignments(image_dataset.size());

        for (size_t i = 0; i < image_dataset.size(); i++)
        {
            double minDistance = std::numeric_limits<double>::max();
            int nearestCluster = -1;

            // Find the nearest cluster center for the current data point
            for (int j = 0; j < no_clusters; j++)
            {
                double distance = euclideanDistance(image_dataset[i].GetImageData(), cluster_centers[j]);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestCluster = j;
                }
            }

            // Assign the data point to the nearest cluster
            assignments[i] = nearestCluster;
        }

        return assignments;
    }
};

#endif // CLUSTER_H