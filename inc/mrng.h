#ifndef MRNG_H
#define MRNG_H

#include <algorithm>
#include <array>
#include <fstream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <set>
#include <random>
#include <iostream>

#include "hash.h"
#include "lsh.h"
#include "mnist.h"

using namespace std;

// MRNG contains the functionality of the Locality-Sensitive Hashing algorithm.
class MRNG
{
private:
    int no_candinates;
    vector<MNIST_Image> images; // The MNIST dataset's images converted to d-vectors.
    LSH lsh;                    // The LSH is going to be used to find the candinates.

public:
    // Create a new instance of LSH.
    MRNG(MNIST _input, int _no_candinates)
    {
        no_candinates = _no_candinates;
        images = _input.GetImages();
        lsh = LSH(_input, 5, 7);
    }

    // Find the nearest neighbour for the query_image
    MNIST_Image FindNearestNeighbor(MNIST_Image query_image)
    {
        // Initialize distance in the images.
        for (auto &image : images)
        {
            auto distance = EuclideanDistance(2, query_image.GetImageData(), image.GetImageData());
            image.SetDist(distance);
        }

        // Initialize the candinates.
        auto candinates = set<MNIST_Image, MNIST_ImageComparator>(images.begin(), images.end());
        auto neighbours = lsh.FindNearestNeighbors(no_candinates, query_image); // Initialize the candinates using LSH.

        return MNIST_Image();
        // auto c_wo_n = set<MNIST_Image, MNIST_ImageComparator>();
        // std::set_difference(candinates.begin(), candinates.end(), neighbours.begin(), neighbours.end(), std::inserter(c_wo_n, c_wo_n.begin()));

        // auto condition = true;
        // for (auto r : c_wo_n)
        // {
        //     for (auto t : neighbours)
        //     {
        //         // consider query image as p
        //         auto pr = EuclideanDistance(2, query_image.GetImageData(), r.GetImageData());
        //         auto pt = EuclideanDistance(2, query_image.GetImageData(), t.GetImageData());
        //         auto tr = EuclideanDistance(2, r.GetImageData(), t.GetImageData());

        //         if (pr > pt && pr > tr)
        //         {
        //             condition = false;
        //             break;
        //         }
        //     }

        //     if (condition)
        //     {
        //         auto pos = neighbours.lower_bound(r);
        //         neighbours.insert(pos, r);
        //     }
        // }

        // auto nn = *(neighbours.begin());

        // return nn;
    }
};

#endif // MRNG_H