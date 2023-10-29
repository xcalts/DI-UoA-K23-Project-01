#ifndef BRUTE_H
#define BRUTE_H

#include <set>

#include "hash.h"
#include "mnist.h"

// BRUTE contains the functionality of the Brute Force algorithm.
class BRUTE
{
private:
    vector<MNIST_Image> images; // The MNIST dataset's images converted to d-vectors.

public:
    // Create a new instance of Brute Force.
    BRUTE(MNIST _input)
    {
        images = _input.GetImages();
    }

    // Find the {no_neighbors} Nearest Neighbors using Brute Force.
    set<MNIST_Image, MNIST_ImageComparator> FindNearestNeighbors(int no_neighbours, MNIST_Image query_image)
    {
        set<MNIST_Image, MNIST_ImageComparator> nearest_neighbors; // Used for sorting the final vector of ANN
        double min_dist = pow(2, 32) - 5;

        for (int i = 0; i < (int)images.size(); i++)
        {
            double dist = EuclideanDistance(2, query_image.GetImageData(), images[i].GetImageData());
            images[i].SetDist(dist);

            if (dist < min_dist)
            {
                if ((int)nearest_neighbors.size() == no_neighbours)
                    nearest_neighbors.erase(--nearest_neighbors.end());

                nearest_neighbors.insert(images[i]);

                set<MNIST_Image, MNIST_ImageComparator>::reverse_iterator itr = nearest_neighbors.rbegin();
                MNIST_Image last_image = *itr;
                min_dist = last_image.GetDist();
            }
        }

        return nearest_neighbors;
    }
};

#endif // BRUTE_H