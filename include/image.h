#ifndef IMAGE_H
#define IMAGE_H

#include <array>
#include <cstdint>
#include <cmath>
#include <sys/types.h>

using namespace std;

class Image
{
private:
    uint index_in_dataset;
    array<uint8_t, 784> image_data;
    double dist;
public:
    /* Constructors */
    Image(): index_in_dataset(0), image_data{}, dist(pow(2, 32) - 5) 
    {

    }
    Image(uint indx, array<uint8_t, 784> data): index_in_dataset(indx), image_data(data), dist(pow(2, 32) - 5)
    {

    }

    /* Accessors */
    uint GetIndex();

    array<uint8_t, 784> GetImageData();

    double GetDist();

    void SetDist(double);

};

struct ImageComparator {
    bool operator() (const Image& a, const Image& b) const {
        Image first_image = a;
        Image second_image = b;
        return first_image.GetDist() < second_image.GetDist();
    }
};

#endif // IMAGE_H