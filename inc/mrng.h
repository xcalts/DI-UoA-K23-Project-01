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
    MNIST_Image FindNearestNeighbor(MNIST_Image p)
    {
        // Find the neighbours using LSH.
        auto Lp = lsh.FindNearestNeighbors(no_candinates, p);

        // Candinates - Neighbours
        set<MNIST_Image, MNIST_ImageComparator> Rp_minus_Lp;
        std::set_difference(
            images.begin(),
            images.end(),
            Lp.begin(),
            Lp.end(),
            inserter(Rp_minus_Lp, Rp_minus_Lp.begin()),
            MNIST_ImageComparator());

        // MRNG Algorithm
        auto condition = true;
        for (auto r : Rp_minus_Lp)
        {
            for (auto t : Lp)
            {
                auto pr = EuclideanDistance(2, p.GetImageData(), r.GetImageData());
                auto pt = EuclideanDistance(2, p.GetImageData(), t.GetImageData());
                auto tr = EuclideanDistance(2, r.GetImageData(), t.GetImageData());

                if (pr > pt && pr > tr)
                {
                    condition = false;
                    break;
                }
            }

            if (condition)
            {
                auto pos = Lp.lower_bound(r);
                Lp.insert(pos, r);
            }
        }

        // The nearest is at indx 0
        auto nn = *(Lp.begin());

        return nn;
    }
};

#endif // MRNG_H