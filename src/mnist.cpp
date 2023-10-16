#include <iostream>
#include <vector>
#include <cstring>

#include "file.h"
#include "mem.h"
#include "mnist.h"

using namespace std;

MNIST::MNIST(const std::string filepath)
{
    file_path = filepath;
    std::vector<uint8_t>
        file_bytes = ReadFileAsBytes(file_path);

    magic_number = ExtractIntFromBytes(file_bytes, 0);
    no_images = ExtractIntFromBytes(file_bytes, 4);
    no_rows = ExtractIntFromBytes(file_bytes, 8);
    no_columns = ExtractIntFromBytes(file_bytes, 12);

    for (size_t i = 0; i < no_images; i++)
    {
        images.push_back(ExtractArrayFromBytes(file_bytes, i * 784));
    }
};

/* Accessors */
std::string MNIST::GetFilePath() { return file_path; }
uint32_t MNIST::GetMagicNumber() { return magic_number; }
uint32_t MNIST::GetImagesCount() { return no_images; }
uint32_t MNIST::GetRowsCount() { return no_rows; }
uint32_t MNIST::GetColumnsCount() { return no_columns; };

/* Functions */

void MNIST::Print()
{
    std::cout << "Magic Number:      " << GetMagicNumber() << std::endl;
    std::cout << "Number of Images:  " << GetImagesCount() << std::endl;
    std::cout << "Number of Rows:    " << GetRowsCount() << std::endl;
    std::cout << "Number of Columns: " << GetColumnsCount() << std::endl;
}