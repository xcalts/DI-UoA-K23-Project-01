#ifndef CLUSTER_H
#define CLUSTER_H

#include <array>
#include <chrono>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <limits>

#include "hash.h"
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
    double executime_time_sec;

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
        auto start = chrono::high_resolution_clock::now();
        initializeClusterCentersKMeansPP();
        assignToNearestClusterLloyd();
        updateClusterCentersMacQueen();
        auto stop = chrono::high_resolution_clock::now();

        executime_time_sec = chrono::duration_cast<chrono::microseconds>(stop - start).count() / 1000000;
    }

    // Function to initialize cluster centers using k-Means++
    void initializeClusterCentersKMeansPP()
    {   
        cout << "Initializing centroids using k-means++..." << endl;

        vector<array<uint8_t, 784>> centers;
        centers.push_back(image_dataset[std::rand() % image_dataset.size()].GetImageData());

        while (centers.size() < no_clusters)
        {
            // Calculate the minimum distance from each data point to the nearest center
            std::vector<double> min_dists_squared(image_dataset.size(), std::numeric_limits<double>::max());
            double min_dists_squared_sum = 0.0;

            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                for (const array<uint8_t, 784> &center : centers)
                {
                    double dist = CalculateDistance(2, image_dataset[i].GetImageData(), center);
                    
                    min_dists_squared[i] = std::min(min_dists_squared[i], pow(dist, 2));
                }

                min_dists_squared_sum += min_dists_squared[i];
            }

            std::vector<double> probabilities(image_dataset.size());
            double sum = 0;
            for(size_t i = 0; i < image_dataset.size(); i++)
            {
                sum += min_dists_squared[i] / min_dists_squared_sum;
                probabilities[i] = min_dists_squared[i] / min_dists_squared_sum;
            }
            
            cout << sum << endl;
            
            // Choose the next center based on the probability proportional to its distance
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<double> random_uniform(0.0, 1.0);
            
            double rand_value = random_uniform(gen);
            double cumulative_probability = 0.0;
            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                cumulative_probability += probabilities[i];
                if (rand_value <= cumulative_probability)
                {
                    centers.push_back(image_dataset[i].GetImageData()); // Add the data point as a new center
                    break;
                }
            }
        }

        cluster_centers = centers; // Return the initialized cluster centers
    }

    // Function to assign each data point to the nearest cluster center using Lloyd's algorithm
    void assignToNearestClusterLloyd()
    {   
        cout << "Assigning points to clusters using Lloyd's algorithm..." << endl;
        for (size_t i = 0; i < image_dataset.size(); i++)
        {
            double minDistance = std::numeric_limits<double>::max();
            int nearestCluster = -1;

            // Find the nearest cluster center for the current data point
            for (int j = 0; j < no_clusters; j++)
            {
                double distance = CalculateDistance(2, image_dataset[i].GetImageData(), cluster_centers[j]);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestCluster = j;
                }
            }

            // Assign the data point to the nearest cluster
            assignments[i] = nearestCluster;
        }
    }

    // Function to update cluster centers using the MacQueen method
    void updateClusterCentersMacQueen()
    {   
        cout << "Updating cluster centroids using MacQueen..." << endl;
        std::vector<array<uint8_t, 784>> updatedCenters(no_clusters);
        std::vector<int> clusterSizes(no_clusters, 0);

        // Calculate the new cluster centers
        for (size_t i = 0; i < image_dataset.size(); i++)
        {
            int cluster = assignments[i];
            clusterSizes[cluster]++;

            for (int j = 0; j < 784; j++)
            {
                updatedCenters[cluster][j] += image_dataset[i].GetImageData()[j];
            }
        }

        // Normalize the cluster centers
        for (int i = 0; i < no_clusters; i++)
        {
            if (clusterSizes[i] > 0)
            {
                for (int j = 0; j < 784; j++)
                {
                    updatedCenters[i][j] /= clusterSizes[i];
                }
            }
        }
    };

    // Function to calculate the Silhouette score for a single cluster
    double silhouetteScoreForCluster(const vector<array<uint8_t, 784>> &cluster)
    {
        double silhouette = 0.0;

        for (size_t i = 0; i < cluster.size(); i++)
        {
            double a = 0.0;                                // Mean intra-cluster distance
            double b = std::numeric_limits<double>::max(); // Mean nearest-cluster distance

            // Calculate mean intra-cluster distance
            for (size_t j = 0; j < cluster.size(); j++)
            {
                if (i != j)
                {
                    a += CalculateDistance(2, cluster[i], cluster[j]);
                }
            }
            if (cluster.size() > 1)
            {
                a /= (cluster.size() - 1);
            }

            // Calculate mean nearest-cluster distance
            for (const array<uint8_t, 784> &otherPoint : cluster)
            {
                double meanDistanceToOtherCluster = 0.0;
                for (size_t j = 0; j < cluster.size(); j++)
                {
                    if (i != j)
                    {
                        meanDistanceToOtherCluster += CalculateDistance(2, otherPoint, cluster[j]);
                    }
                }
                if (cluster.size() > 1)
                {
                    meanDistanceToOtherCluster /= (cluster.size() - 1);
                }
                b = std::min(b, meanDistanceToOtherCluster);
            }

            // Calculate Silhouette score for the current data point
            double s = (b - a) / std::max(a, b);
            silhouette += s;
        }

        return silhouette / cluster.size();
    }

    // Function to calculate the Silhouette score for all clusters and average
    double silhouetteScore(const vector<vector<array<uint8_t, 784>>> &clusters)
    {
        double averageSilhouette = 0.0;

        for (const std::vector<array<uint8_t, 784>> &cluster : clusters)
        {
            double clusterSilhouette = silhouetteScoreForCluster(cluster);
            averageSilhouette += clusterSilhouette;
        }

        return averageSilhouette / clusters.size();
    }

    stringstream getResults()
    {
        stringstream results;

        results << "Algorithm: Lloyds" << endl;

        // Basically print cluster information
        for (size_t i = 0; i < no_clusters; i++)
        {
            vector<int> assignments_per_cluster;

            for (size_t j = 0; j < image_dataset.size(); j++)
                if (assignments[j] == i)
                    assignments_per_cluster.push_back(image_dataset[j].GetIndex());

            results << "CLUSTER-" << i + 1 << " {size: " << assignments_per_cluster.size() << ", centroid:[";
            for (size_t k = 0; k < assignments_per_cluster.size(); k++)
                if (k < assignments_per_cluster.size() - 1)
                    results << assignments_per_cluster[k] << " ";
                else
                    results << assignments_per_cluster[k]; // last
            results << "]" << endl;
        }
        results << "clustering_time: " << executime_time_sec << " // in seconds" << endl;

        // vector<vector<array<uint8_t, 784>>> clusters(no_clusters);
        // for (size_t i = 0; i < image_dataset.size(); i++)
        //     clusters[assignments[i]].push_back(image_dataset[i].GetImageData());

        // results << "Silhouette: [";
        // for (size_t i = 0; i < no_clusters; i++)
        //     if (i < no_clusters - 1)
        //         results << silhouetteScoreForCluster(clusters[i]) << ",";
        //     else
        //         results << silhouetteScoreForCluster(clusters[i]) << "]" << endl; // last

        return results;
    }
};

#endif // CLUSTER_H