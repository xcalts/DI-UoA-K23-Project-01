#ifndef MNIST_H
#define MNIST_H

#include <array>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

class Image
{
private:
    /* Fields */
    uint index_in_dataset;
    array<uint8_t, 784> image_data;
    double dist;
    int id;

public:
    /* Constructors */
    Image() : index_in_dataset(0), image_data{}, dist(pow(2, 32) - 5), id(-1)
    {
    }
    Image(uint indx, array<uint8_t, 784> data) : index_in_dataset(indx), image_data(data), dist(pow(2, 32) - 5), id(-1)
    {
    }

    /* Accessors */
    uint GetIndex()
    {
        return index_in_dataset;
    }

    array<uint8_t, 784> GetImageData()
    {
        return image_data;
    }

    double GetDist()
    {
        return dist;
    }

    void SetDist(double distance)
    {
        dist = distance;
    }

    int GetId()
    {
        return id;
    }

    void SetId(int Id)
    {
        id = Id;
    }
};

struct ImageComparator
{
    bool operator()(const Image &a, const Image &b) const
    {
        Image first_image = a;
        Image second_image = b;
        return first_image.GetDist() < second_image.GetDist();
    }
};

class MNIST
{
private:
    /* Fields */
    string file_path;
    uint32_t magic_number;
    uint32_t no_images;
    uint32_t no_rows;
    uint32_t no_columns;
    vector<Image> images; // 28*28 dimensions

    uint32_t ExtractIntFromBytes(const vector<uint8_t> &bytes, size_t offset)
    {
        if (bytes.size() < offset + 4)
        {
            throw runtime_error("Vector does not contain enough bytes to convert to a 32-bit integer at the specified offset.");
        }

        uint32_t integerVal = 0;

        // Extract 4 bytes and convert to a 32-bit integer in a reverse byte order
        for (int i = 0; i < 4; i++)
        {
            integerVal |= static_cast<uint32_t>(bytes[offset + 3 - i]) << (i * 8);
        }

        return integerVal;
    }

    array<uint8_t, 784> ExtractArrayFromBytes(const vector<uint8_t> &bytes, size_t offset)
    {
        if (bytes.size() < offset + 784)
        {
            throw runtime_error("Vector does not contain enough bytes to extract the specified size.");
        }

        array<uint8_t, 784> data;

        for (size_t i = 0; i < 784; i++)
        {
            data[783 - i] = bytes[offset + i];
        }

        return data;
    }

    vector<uint8_t> ReadFileAsBytes(const string &file_path)
    {
        vector<uint8_t> file_bytes;

        // Open the file in binary mode
        ifstream file(file_path, ios::binary);

        // Check if the file is open
        if (!file.is_open())
        {
            throw runtime_error("Failed to open the file: " + file_path + "\n");
        }

        // Read the file into a vector of bytes
        file.seekg(0, ios::end); // Move to the end of the file to determine its size
        size_t file_size = file.tellg();
        file_bytes.resize(file_size);
        file.seekg(0, ios::beg); // Move back to the beginning of the file
        file.read(reinterpret_cast<char *>(file_bytes.data()), file_size);

        // Check if the file was read successfully
        if (!file)
        {
            throw runtime_error("Failed to read the file: " + file_path + "\n");
        }

        // Close the file
        file.close();

        return file_bytes;
    }

public:
    /* Constructor */
    MNIST(const string filepath)
    {
        file_path = filepath;
        vector<uint8_t> file_bytes = ReadFileAsBytes(file_path);

        magic_number = ExtractIntFromBytes(file_bytes, 0);
        no_images = ExtractIntFromBytes(file_bytes, 4);
        no_rows = ExtractIntFromBytes(file_bytes, 8);
        no_columns = ExtractIntFromBytes(file_bytes, 12);

        for (size_t i = 0; i < no_images; i++)
        {
            images.push_back(Image((uint)i, ExtractArrayFromBytes(file_bytes, 16 + i * 784)));
        }
    };

    /* Accessors */
    string GetFilePath() { return file_path; }

    uint32_t GetMagicNumber() { return magic_number; }

    uint32_t GetImagesCount() { return no_images; }

    uint32_t GetRowsCount() { return no_rows; }

    uint32_t GetColumnsCount() { return no_columns; }

    vector<Image> GetImages() { return images; }

    /* Functions */
    void Print()
    {
        cout << "MNIST File:        " << GetFilePath() << endl;
        cout << "Magic Number:      " << GetMagicNumber() << endl;
        cout << "Number of Images:  " << GetImagesCount() << endl;
        cout << "Number of Rows:    " << GetRowsCount() << endl;
        cout << "Number of Columns: " << GetColumnsCount() << endl;
    }

    void PrintImage(size_t indx)
    {
        if (indx > images.size() - 1)
            throw runtime_error("Failed to draw the image at index " + indx);

        array<uint8_t, 784> image = images.at(indx).GetImageData();

        for (int32_t i = no_rows-1; i >= 0; i--)
        {
            for (int32_t j = no_columns-1; j >= 0; j--)
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
};

#endif // MNIST_H