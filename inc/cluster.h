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

#include "mnist.h"
#include "cube.h"
#include "lsh.h"

#define START_RANGE 10000

using namespace std;

enum Method
{
    LLOYD_METHOD,
    LSH_METHOD,
    HYPERCUBE_METHOD,
    UNKNOWN
};

// Cluster contains the functionality of Clustering methods.
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
    MNIST dataset;
    vector<MNIST_Image> image_dataset;
    vector<IMAGE_DATA> cluster_centers;
    vector<IMAGE_DATA> unnormalized_cluster_centers;
    unordered_map<int, vector<MNIST_Image>> clusters;
    vector<int> assignments;
    double executime_time_sec;
    Method method;
    int range;
    int assigned;

    // Function to calculate the Euclidean distance between two data points
    double euclideanDistance(const IMAGE_DATA &a, const IMAGE_DATA &b)
    {
        double sum = 0.0;
        for (int i = 0; i < 784; i++)
        {
            sum += std::pow(a[i] - b[i], 2);
        }
        return sqrt(sum);
    }

    Method parseMethod(const std::string input)
    {
        if (input == "lloyd")
        {
            return LLOYD_METHOD;
        }
        else if (input == "lsh")
        {
            return LSH_METHOD;
        }
        else if (input == "hypercube")
        {
            return HYPERCUBE_METHOD;
        }
        else
        {
            cout << "Failed to parse the provided method." << endl;
            return UNKNOWN;
        }
    }

public:
    /* Constructors*/
    Cluster(int _no_clusters,
            int _no_hash_tables,
            int _no_hash_functions,
            int _no_max_hypercubes,
            int _no_dim_hypercubes,
            int _no_probes,
            MNIST _dataset,
            string _method)
    {
        no_clusters = _no_clusters;
        no_hash_tables = _no_hash_tables;
        no_hash_functions = _no_hash_functions;
        no_max_hypercubes = _no_max_hypercubes;
        no_dim_hypercubes = _no_dim_hypercubes;
        no_probes = _no_probes;
        dataset = _dataset;
        image_dataset = _dataset.GetImages();
        assignments = vector<int>(image_dataset.size());
        method = parseMethod(_method);
        range = START_RANGE;
        assigned = 0;

        unnormalized_cluster_centers = vector<IMAGE_DATA>(no_clusters);
        for (int i = 0; i < no_clusters; i++)
        {
            unnormalized_cluster_centers[i].fill(0);
        }

        Initialization();
    }

    /* Initialization */
    void Initialization()
    {
        auto start = chrono::high_resolution_clock::now();

        initializeClusterCentersKMeansPP();

        uint changes = 1;
        int it = 0;
        bool isFirst = true;

        while ((((double)changes / (double)image_dataset.size()) > 0.1 && it < 15) || isFirst)
        {

            isFirst = false;

            cout << "Changes: " << changes << ", Range: " << range << ", Assigned: " << assigned << endl;
            changes = 0;
            switch (method)
            {
            case LLOYD_METHOD:
                for (int i = 0; i < no_clusters; i++)
                {
                    unnormalized_cluster_centers[i].fill(0.0);
                }
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
                return;
            }

            if ((method == LSH_METHOD || method == HYPERCUBE_METHOD) &&
                (image_dataset.size() - assigned) / image_dataset.size() >= 0.01)
            {

                break;
            }

            it++;
        }

        cout << ((double)changes / (double)image_dataset.size()) << endl;

        auto stop = chrono::high_resolution_clock::now();
        executime_time_sec = chrono::duration_cast<chrono::microseconds>(stop - start).count() / 1000000;

        for (int k = 0; k < no_clusters; k++)
        {
            cout << k << ": " << clusters[k].size() << endl;
        }
    }

    // Function to initialize cluster centers using k-Means++
    void initializeClusterCentersKMeansPP()
    {
        vector<IMAGE_DATA> centers;
        centers.push_back(image_dataset[std::rand() % image_dataset.size()].GetImageData());

        while (centers.size() < no_clusters)
        {
            // Calculate the minimum distance from each data point to the nearest center
            std::vector<double> distances(image_dataset.size(), std::numeric_limits<double>::max());
            for (size_t i = 0; i < image_dataset.size(); i++)
            {
                for (const IMAGE_DATA &center : centers)
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

        cluster_centers = centers; // Return the initialized cluster centers
    }

    // Function to assign each data point to the nearest cluster center using range search
    uint assignToNearestClusterRange(Method method)
    {
        cout << assigned << endl;
        uint changes = 0;

        // Need to create images from centers
        vector<MNIST_Image> center_images(no_clusters);
        for (int i = 0; i < no_clusters; i++)
        {
            MNIST_Image image = MNIST_Image(i, cluster_centers[i]);
            center_images[i] = image;
        }

        // Vector that will keep count of conflicts
        vector<vector<int>> conflicts(image_dataset.size(), vector<int>(0));

        switch (method)
        {
        case LSH_METHOD:
        {
            // For each center
            // For given range, execute given LSH RadiusSearch using the one center as query
            LSH lsh = LSH(dataset, no_hash_functions, no_hash_tables);
            for (int i = 0; i < no_clusters; i++)
            {
                // Execute given LSH RadiusSearch using the one center as query
                set<MNIST_Image, MNIST_ImageComparator> neighbors_in_radius = lsh.RadiusSearch(center_images[i], range);

                for (set<MNIST_Image, MNIST_ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
                {
                    MNIST_Image neighbor = *it;
                    conflicts[neighbor.GetIndex()].push_back(i);
                }
            }

            break;
        }
        case HYPERCUBE_METHOD:
        {
            // For each center
            // For given range, execute given Hypercube RadiusSearch using the one center as query
            Hypercube hypercube = Hypercube(dataset, no_dim_hypercubes, no_max_hypercubes, no_probes);
            for (int i = 0; i < no_clusters; i++)
            {
                // Execute given Hypercube RadiusSearch using the one center as query
                set<MNIST_Image, MNIST_ImageComparator> neighbors_in_radius = hypercube.RadiusSearch(center_images[i], range);

                for (set<MNIST_Image, MNIST_ImageComparator>::iterator it = neighbors_in_radius.begin(); it != neighbors_in_radius.end(); ++it)
                {
                    MNIST_Image neighbor = *it;
                    conflicts[neighbor.GetIndex()].push_back(i);
                }
            }

            break;
        }
        }

        // Resolve conflicts, aka points that fall into 2 radiuses, by calculating distance between point and centers
        for (int i = 0; i < image_dataset.size(); i++)
        {
            // cout << i << ", " << conflicts[i].size() << endl;

            if (conflicts[i].size() == 1)
            {
                // cout << i << " > " << conflicts[i][0] << endl;

                int prev_cluster = assignments[i];
                int new_cluster = conflicts[i][0];

                // if prev_cluster != new_cluster (happens if it was previously unassigned, or if cluster center's changed dramatically)
                if (prev_cluster != new_cluster)
                {
                    changes++;

                    // Remove from previous cluster
                    if (prev_cluster != -1)
                    {
                        for (int j = 0; j < clusters[prev_cluster].size(); j++)
                        {
                            if (image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex())
                            {
                                vector<MNIST_Image>::iterator indx = clusters[prev_cluster].begin() + j;
                                clusters[prev_cluster].erase(indx);
                            }
                        }
                    }
                    else
                    {
                        assigned++;
                    }

                    // Add to new cluster
                    assignments[i] = new_cluster;
                    clusters[new_cluster].push_back(image_dataset[i]);
                }
            }
            else if (conflicts[i].size() > 1)
            {
                // cout << i << ": " << conflicts[i].size() << endl;

                // Compare distances between conflicting clusters
                int prev_cluster = assignments[i];
                double min_dist = std::numeric_limits<double>::max();
                int nearest_cluster = -1;
                for (int j = 0; j < conflicts[i].size(); j++)
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
                if (prev_cluster != nearest_cluster)
                {
                    changes++;

                    // Remove from previous cluster
                    if (prev_cluster != -1)
                    {
                        for (int j = 0; j < clusters[prev_cluster].size(); j++)
                        {
                            if (image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex())
                            {
                                vector<MNIST_Image>::iterator indx = clusters[prev_cluster].begin() + j;
                                clusters[prev_cluster].erase(indx);
                            }
                        }
                    }
                    else
                    {
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
            if (prev_cluster != nearest_cluster)
            {

                changes++;

                // Remove image from previous cluster
                if (prev_cluster != -1)
                {
                    for (int j = 0; j < clusters[prev_cluster].size(); j++)
                    {
                        if (image_dataset[i].GetIndex() == clusters[prev_cluster][j].GetIndex())
                        {
                            vector<MNIST_Image>::iterator indx = clusters[prev_cluster].begin() + j;
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

    // Function to update cluster centers using the MacQueen method
    void updateClusterCentersMacQueen()
    {
        std::vector<IMAGE_DATA> updatedCenters(no_clusters);
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
    double silhouetteScoreForCluster(const vector<IMAGE_DATA> &cluster)
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
            for (const IMAGE_DATA &otherPoint : cluster)
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
    double silhouetteScore(const vector<vector<IMAGE_DATA>> &clusters)
    {
        double averageSilhouette = 0.0;

        for (const std::vector<IMAGE_DATA> &cluster : clusters)
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

        // vector<vector<IMAGE_DATA>> clusters(no_clusters);
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