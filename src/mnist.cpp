#include <iostream>
#include <vector>
#include <cstring>

#include "file.h"
#include "mem.h"
#include "image.h"
#include "mnist.h"

using namespace std;

MNIST::MNIST(const string filepath)
{
    file_path = filepath;
    vector<uint8_t> file_bytes = ReadFileAsBytes(file_path);

    magic_number = ExtractIntFromBytes(file_bytes, 0);
    no_images = ExtractIntFromBytes(file_bytes, 4);
    no_rows = ExtractIntFromBytes(file_bytes, 8);
    no_columns = ExtractIntFromBytes(file_bytes, 12);

    for (size_t i = 0; i < no_images; i++)
    {
        images.push_back( Image((uint) i, ExtractArrayFromBytes(file_bytes, 16 + i * 784)) );
    }
}

/* Accessors */
string MNIST::GetFilePath() { return file_path; }
uint32_t MNIST::GetMagicNumber() { return magic_number; }
uint32_t MNIST::GetImagesCount() { return no_images; }
uint32_t MNIST::GetRowsCount() { return no_rows; }
uint32_t MNIST::GetColumnsCount() { return no_columns; }
vector<Image> MNIST::GetImages() { return images; }

/* Functions */
void MNIST::Print()
{
    cout << "MNIST File:        " << GetFilePath() << endl;
    cout << "Magic Number:      " << GetMagicNumber() << endl;
    cout << "Number of Images:  " << GetImagesCount() << endl;
    cout << "Number of Rows:    " << GetRowsCount() << endl;
    cout << "Number of Columns: " << GetColumnsCount() << endl;
}

void MNIST::PrintImage(size_t indx)
{
    if (indx > images.size() - 1)
        throw runtime_error("Failed to draw the image at index " + indx);

    array<uint8_t, 784> image = images.at(indx).GetImageData();

    for (uint32_t i = 0; i < no_rows; ++i)
    {
        for (uint32_t j = 0; j < no_columns; ++j)
        {
            int pixelValue = image[i * no_columns + j];
            char displayChar = '#';

            // Use ' ' for white and '#' for black based on the pixel value
            if (pixelValue < 128)
            {
                displayChar = ' '; // Black
            }

            std::cout << displayChar;
        }
        std::cout << std::endl;
    }
}