#ifndef MNIST_H
#define MNIST_H

#include <array>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class MNIST
{
private:
    string file_path;
    uint32_t magic_number;
    uint32_t no_images;
    uint32_t no_rows;
    uint32_t no_columns;
    vector<array<uint8_t, 784>> images; // 28*28 dimensions

public:
    /* Constructor */
    MNIST(const string);

    /* Accessors */
    string GetFilePath();
    uint32_t GetMagicNumber();
    uint32_t GetImagesCount();
    uint32_t GetRowsCount();
    uint32_t GetColumnsCount();
    vector<array<uint8_t, 784>> GetImages();

    /* Functions */
    void Print();
};

#endif // MNIST_H