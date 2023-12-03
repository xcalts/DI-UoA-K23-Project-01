#ifndef MRNG_H
#define MRNG_H

#include <deque>
#include <vector>
#include <list>
#include <set>

#include "hash.h"
#include "lsh.h"
#include "mnist.h"

using namespace std;

// MRNG contains the functionality of the algorithm.
class MRNG
{
private:
    int no_candidates;
    MNIST input;
    vector<MNIST_Image> images; // The MNIST dataset's images converted to d-vectors.
    LSH lsh;                    // The LSH is going to be used to find the candinates.
    list<int> *graph;           // Graph implementation using adjacency list.

public:
    // Create a new instance of LSH.
    MRNG(MNIST _input, int _no_candidates)
    {
        no_candidates = _no_candidates;
        input = _input;
        images = _input.GetImages();
        graph = new list<int>[_input.GetImagesCount()];
    }

    void Initialization()
    {
        lsh = LSH(input, 10, 15);
        cout << "[i] Initializing MRNG Construction." << endl;
        printProgress(0.0);
        for (MNIST_Image p : images)
        {
            // Create another copy of the original vector
            auto _images = images;
            // Create Rp: S - p
            _images.erase(std::remove(_images.begin(), _images.end(), p), _images.end());
            set<MNIST_Image, MNIST_ImageComparator> Rp(_images.begin(), _images.end());

            // Find the neighbours using LSH.
            auto Lp = lsh.FindNearestNeighbors(no_candidates, p);

            // Candidates - Neighbours
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

            for (auto image : Lp)
            {
                graph[p.GetIndex()].push_back(image.GetIndex());
            }

            printProgress((double)p.GetIndex() / (double)images.size());
        }

        printProgress(1.0);
        cout << endl
             << "[i] Finished MRNG Construction." << endl;
    }

    // Find the nearest neighbour for the query_image
    set<MNIST_Image, MNIST_ImageComparator> FindNearestNeighbors(int no_nearest_neighbours, MNIST_Image query_image)
    {
        set<MNIST_Image, MNIST_ImageComparator> possible_nearest_neighbors; // Used for sorting the final vector of ANN
        vector<MNIST_Image> unchecked_nodes;                                // Store unchecked nodes

        random_device rd;
        mt19937 gen(rd());

        uniform_int_distribution<int> random_image_index(0, images.size() - 1);

        // Select a graph's node to start at random
        int index = random_image_index(gen);
        MNIST_Image node_image = images[index];
        double dist = EuclideanDistance(2, query_image.GetImageData(), node_image.GetImageData());
        node_image.SetDist(dist);

        unchecked_nodes.push_back(node_image);

        for (int i = 1; i < no_candidates; i++)
        {
            // Check the first unchecked node
            MNIST_Image node_to_check = *unchecked_nodes.begin();
            unchecked_nodes.erase(unchecked_nodes.begin());
            possible_nearest_neighbors.insert(node_to_check);

            for (int neighbor_index : graph[node_to_check.GetIndex()])
            {
                MNIST_Image neighbor_image = images[neighbor_index];
                dist = EuclideanDistance(2, query_image.GetImageData(), neighbor_image.GetImageData());
                neighbor_image.SetDist(dist);

                unchecked_nodes.push_back(neighbor_image);
            }

            // Sort unchecked nodes
            sort(
                unchecked_nodes.begin(),
                unchecked_nodes.end(),
                MNIST_ImageComparator());
        }

        set<MNIST_Image, MNIST_ImageComparator> nearest_neighbors;

        int nn_num = 0;
        for (MNIST_Image nn : possible_nearest_neighbors)
        {
            if (nn_num == no_nearest_neighbours)
            {
                break;
            }

            nearest_neighbors.insert(nn);
            nn_num++;
        }

        return nearest_neighbors;
    }

    void PrintGraph()
    {
        cout << "DEBUG: Printing Graph." << endl;

        for (MNIST_Image query_image : images)
        {
            cout << query_image.GetIndex() << "\n";

            for (int i : graph[query_image.GetIndex()])
            {
                cout << "Edge(" << query_image.GetIndex() << ", " << i << ")\n";
            }
        }

        cout << "DEBUG: Finished printing Graph." << endl;
    }
};

#endif // MRNG_H