#ifndef MNIST_H
#define MNIST_H

#include <array>
#include <iostream>
#include <vector>
#include <string>

#include "image.h"

using namespace std;

class MNIST
{
private:
    string file_path;
    uint32_t magic_number;
    uint32_t no_images;
    uint32_t no_rows;
    uint32_t no_columns;
    vector<Image> images; // 28*28 dimensions
public:
    /* Constructor */
    MNIST(const string);

    /* Accessors */
    string GetFilePath();
    uint32_t GetMagicNumber();
    uint32_t GetImagesCount();
    uint32_t GetRowsCount();
    uint32_t GetColumnsCount();
    vector<Image> GetImages();

    /* Functions */
    void Print();
    void PrintImage(size_t);
};

#endif // MNIST_H