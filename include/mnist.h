#ifndef MNIST_H
#define MNIST_H

#include <array>
#include <iostream>
#include <vector>
#include <string>

class MNIST
{
private:
    std::string file_path;
    uint32_t magic_number;
    uint32_t no_images;
    uint32_t no_rows;
    uint32_t no_columns;
    std::vector<std::array<uint8_t, 784>> images; // 28*28 dimensions

public:
    /* Constructor */
    MNIST(const std::string);

    /* Accessors */
    std::string GetFilePath();
    uint32_t GetMagicNumber();
    uint32_t GetImagesCount();
    uint32_t GetRowsCount();
    uint32_t GetColumnsCount();

    /* Functions */
    void Print();
};

#endif // MNIST_H