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
#include <functional>
#include <unordered_map>
#include <ctime>

#include "hash.h"
#include "lsh.h"
#include "cube.h"
#include "mnist.h"

#define START_RANGE 10000

using namespace std;

enum Method { LLOYD_METHOD, LSH_METHOD, HYPERCUBE_METHOD, UNKNOWN };

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
    vector<array<uint8_t, 784>> unnormalized_cluster_centers;
    vector<int> assignments;
    unordered_map<int, vector<Image>> clusters;
    double executime_time_sec;
    string method;
    int range;
    int assigned;

    Method parseMethod(const std::string& input) {
        if (input == "lloyd") {
            return LLOYD_METHOD;
        } else if (input == "lsh") {
            return LSH_METHOD;
        } else if (input == "hypercube") {
            return HYPERCUBE_METHOD;
        } else {
            return UNKNOWN;
        }
    }

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
            vector<Image> _image_dataset,
            string _method)
    {
        no_clusters = _no_clusters;
        no_hash_tables = _no_hash_tables;
        no_hash_functions = _no_hash_functions;
        no_max_hypercubes = _no_max_hypercubes;
        no_dim_hypercubes = _no_dim_hypercubes;
        no_probes = _no_probes;
        image_dataset = _image_dataset;
        assignments = vector<int>(image_dataset.size(), -1);
        method = _method;
        range = START_RANGE;
        assigned = 0;

        unnormalized_cluster_centers = vector<array<uint8_t, 784>>(no_clusters);
        for(int i = 0; i < no_clusters; i++) {
            unnormalized_cluster_centers[i].fill(0);
        }

        Initialization();
    }

    /* Initialization k-means++ */
    void Initialization()
    {
        auto start = chrono::high_resolution_clock::now();
        
        initializeClusterCentersKMeansPP();
        
        uint changes = 1;
        int i = 0;
        while(changes > 0) {
        // while(i++ < 2) {
            cout << "Changes: " << changes << ", Range: " << range << ", Assigned: " << assigned << endl;
            changes = 0;
            switch(parseMethod(method))
            {
                case LLOYD_METHOD:
                    changes = assignToNearestClusterLloyd();
                    break;
                case LSH_METHOD:
                    changes = assignToNearestClusterRange(LSH_METHOD);
                    updateClusterCentersMacQueen();
                    break;
                case HYPERCUBE_METHOD:
                    changes = assignToNearestClusterRange(HYPERCUBE_METHOD);
                    updateClusterCentersMacQueen();
                    break;
                default:
                    cout << "Not Lloyd's" << endl;
                    break;
            }
        }
        
        auto stop = chrono::high_resolution_clock::now();
        executime_time_sec = chrono::duration_cast<chrono::microseconds>(stop - start).count() / 1000000;
        
        for(int k = 0; k < no_clusters; k++) {
            cout << k << ": " << clusters[k].size() << endl;

        //     for (int32_t i = 27; i >= 0; i--)
        //     {
        //         for (int32_t j = 27; j >= 0; j--)
        //         {
        //             int pixelValue = cluster_centers[k][i * 28 + j];
        //             char displayChar = '#';

        //             // Use ' ' for white and '#' for black based on the pixel value
        //             if (pixelValue < 128)
        //             {
        //                 displayChar = ' '; // Black
        //             }

        //             cout << displayChar;
        //         }
        //         cout << '\n';
        //     }
        }
    }

    // Function to initialize cluster centers using k-Means++
    void initializeClusterCentersKMeansPP()
    {   
        cout << "Initializing centroids using k-means++..." << endl;

        vector<array<uint8_t, 784>> centers;
        srand(time(0));
        int rand_indx = std::rand() % image_dataset.size();
        assignments[rand_indx] = 0;
        clusters[0].push_back(image_dataset[rand_indx]);
        centers.push_back(image_dataset[rand_indx].GetImageData());

        while (centers.size() < no_clusters)
        {
            cout << "Centroid Incoming... " << centers.size() << endl;
            // Calculate the minimum distance from each data point to the nearest center
            std::vector<double> min_dists_squared(image_dataset.size(), std::numeric_limits<double>::max());
            double min_dists_squared_sum = 0.0;

            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                for (const array<uint8_t, 784> &center : centers)
                {
                    double dist = euclideanDistance(image_dataset[i].GetImageData(), center);
                    
                    min_dists_squared[i] = std::min(min_dists_squared[i], pow(dist, 2));
                }

                min_dists_squared_sum += min_dists_squared[i];
            }

            std::vector<double> probabilities(image_dataset.size());
            for(size_t i = 0; i < image_dataset.size(); i++)
            {
                probabilities[i] = min_dists_squared[i] / min_dists_squared_sum;
            }
            
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
                    assignments[i] = centers.size();
                    clusters[0].push_back(image_dataset[rand_indx]);
                    centers.push_back(image_dataset[i].GetImageData()); // Add the data point as a new center
                    break;
                }
            }
        }

        unnormalized_cluster_centers = centers;
        cluster_centers = centers; // Return the initialized cluster centers
    }

    // Function to assign each data point to the nearest cluster center using Lloyd's algorithm
    uint assignToNearestClusterLloyd()
    {   
        uint changes = 0;
        cout << "Assigning points to clusters using Lloyd's algorithm..." << endl;
        for (size_t i = 0; i < image_dataset.size(); i++)
        {
            
            int prev_cluster = assignments[i];
            double min_distance = std::numeric_limits<double>::max();
            int nearest_cluster = -1;

            // Find the nearest cluster center for the current data point
            for (int j = 0; j < no_clusters; j++)
            {
                double distance = euclideanDistance(image_dataset[i].GetImageData(), cluster_centers[j]);
                // cout << distance << ", " << min_distance << ", " << j << endl;
                if (distance < min_distance)
                {
                    min_distance = distance;
                    nearest_cluster = j;
                }
            }
            
            // Assign the data point to the nearest cluster
            if(prev_cluster != nearest_cluster) {

                changes++;
                
                // Remove image from previous cluster
                if(prev_cluster != -1) {
                    for(int j = 0; j < clusters[prev_cluster].size(); j++) {
                        if(image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex()) {
                            vector<Image>::iterator indx = clusters[prev_cluster].begin() + j;
                            clusters[prev_cluster].erase(indx);
                        }
                    }
                }
                
                // Assign image to nearest cluster
                assignments[i] = nearest_cluster;
                clusters[nearest_cluster].push_back(image_dataset[i]);
                int cluster_size = clusters[nearest_cluster].size();

                // Update nearest cluster's center       
                if (cluster_size > 0)
                {
                    for (int j = 0; j < 784; j++)
                    {   
                        unnormalized_cluster_centers[nearest_cluster][j] += image_dataset[i].GetImageData()[j];
                        cluster_centers[nearest_cluster][j] = unnormalized_cluster_centers[nearest_cluster][j] / cluster_size;
                    }      
                }
                

            }
        }

        return changes;
    }

    // Function to assign each data point to the nearest cluster center using range search
    uint assignToNearestClusterRange(Method method)
    {
        cout << assigned << endl;
        uint changes = 0;

        // Need to create images from centers
        vector<Image> center_images(no_clusters);
        for(int i = 0; i < no_clusters; i++)
        {
            Image image = Image(i, cluster_centers[i]);
            center_images[i] = image;
        }

        // Vector that will keep count of conflicts
        vector<vector<int>> conflicts(image_dataset.size(), vector<int>(0));
        
        switch(method)
        {
            case LSH_METHOD: 
            {
                // For each center
                // For given range, execute given LSH RadiusSearch using the one center as query
                LSH lsh = LSH(image_dataset, center_images, "", no_hash_functions, no_hash_tables, range);
                for(int i = 0; i < no_clusters; i++)
                {
                    // Execute given LSH RadiusSearch using the one center as query
                    set<Image, ImageComparator> neighbors_in_radius = lsh.RadiusSearch(center_images[i]);
                
                    for (set<Image, ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
                    {
                        Image neighbor = *it;
                        conflicts[neighbor.GetIndex()].push_back(i);
                    }
                }

                break;
            }
            case HYPERCUBE_METHOD: 
            {
                // For each center
                // For given range, execute given Hypercube RadiusSearch using the one center as query
                Hypercube hypercube = Hypercube(image_dataset, center_images, "", no_dim_hypercubes, no_max_hypercubes, no_probes, 0, range);
                for(int i = 0; i < no_clusters; i++)
                {
                    // Execute given Hypercube RadiusSearch using the one center as query
                    set<Image, ImageComparator> neighbors_in_radius = hypercube.RadiusSearch(center_images[i]);
                
                    for (set<Image, ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
                    {
                        Image neighbor = *it;
                        conflicts[neighbor.GetIndex()].push_back(i);
                    }
                }

                break;
            }
            default: 
            {
                break;
            }
        }

        // Resolve conflicts, aka points that fall into 2 radiuses, by calculating distance between point and centers
        for(int i = 0; i < image_dataset.size(); i++)
        {
            // cout << i << ", " << conflicts[i].size() << endl;

            if(conflicts[i].size() == 1) {
                // cout << i << " > " << conflicts[i][0] << endl;

                int prev_cluster = assignments[i];
                int new_cluster = conflicts[i][0];

                // if prev_cluster != new_cluster (happens if it was previously unassigned, or if cluster center's changed dramatically)
                if(prev_cluster != new_cluster) {
                    changes++;

                    // Remove from previous cluster
                    if(prev_cluster != -1) {
                        for(int j = 0; j < clusters[prev_cluster].size(); j++) 
                        {
                            if(image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex()) {
                                vector<Image>::iterator indx = clusters[prev_cluster].begin() + j;
                                clusters[prev_cluster].erase(indx);
                            }
                        }
                    } else {
                        assigned++;
                    }

                    // Add to new cluster
                    assignments[i] = new_cluster;
                    clusters[new_cluster].push_back(image_dataset[i]);
                }

            } else if(conflicts[i].size() > 1) {
                // cout << i << ": " << conflicts[i].size() << endl;

                // Compare distances between conflicting clusters
                int prev_cluster = assignments[i];
                double min_dist = std::numeric_limits<double>::max();
                int nearest_cluster = -1;
                for(int j = 0; j < conflicts[i].size(); j++)
                {
                    // cout << conflicts[i][j] << endl;

                    double distance = euclideanDistance(image_dataset[i].GetImageData(), cluster_centers[conflicts[i][j]]);
                    if (distance < min_dist)
                    {
                        min_dist = distance;
                        nearest_cluster = conflicts[i][j];
                    }
                }

                // cout << "Nearest Cluster: " << nearest_cluster << ", Distance: " << min_dist << endl;

                // if prev_cluster != nearest_cluster
                if(prev_cluster != nearest_cluster) {
                    changes++;

                    // Remove from previous cluster
                    if(prev_cluster != -1) {
                        for(int j = 0; j < clusters[prev_cluster].size(); j++) 
                        {
                            if(image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex()) {
                                vector<Image>::iterator indx = clusters[prev_cluster].begin() + j;
                                clusters[prev_cluster].erase(indx);
                            }
                        }
                    } else {
                        assigned++;
                    }

                    // Add to nearest_cluster
                    assignments[i] = nearest_cluster;
                    clusters[nearest_cluster].push_back(image_dataset[i]);
                }
                
            }
        }

        range *= 2;

        return changes;
    }

    // Function to update cluster centers using the MacQueen method
    void updateClusterCentersMacQueen()
    {
        cout << "Updating cluster centers..." << endl;

        std::vector<array<uint8_t, 784>> updatedCenters(no_clusters);
        std::vector<int> clusterSizes(no_clusters, 0);

        // Calculate the new cluster centers
        for (size_t i = 0; i < image_dataset.size(); i++)
        {
            int cluster = assignments[i];
            if(cluster == -1)
                continue;

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

        cluster_centers = updatedCenters;

        for(int k = 0; k < no_clusters; k++) {
            for (int32_t i = 27; i >= 0; i--)
            {
                for (int32_t j = 27; j >= 0; j--)
                {
                    int pixelValue = updatedCenters[k][i * 28 + j];
                    char displayChar = '#';

                    // Use ' ' for white and '#' for black based on the pixel value
                    if (pixelValue < 128)
                    {
                        displayChar = ' '; // Black
                    }

                    cout << displayChar;
                }
                cout << '\n';
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
                    a += euclideanDistance(cluster[i], cluster[j]);
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
                        meanDistanceToOtherCluster += euclideanDistance(otherPoint, cluster[j]);
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