#include <array>
#include <cstdint>

#include "image.h"

uint Image::GetIndex()
{
    return index_in_dataset;
}

array<uint8_t, 784> Image::GetImageData()
{
    return image_data;
}

double Image::GetDist()
{
    return dist;
}

void Image::SetDist(double distance)
{
    dist = distance;
}