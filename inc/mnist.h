#ifndef MNIST_H
#define MNIST_H

#include <array>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

#define DIMENSIONS 784 // 28 * 28

typedef array<double, DIMENSIONS> IMAGE_DATA;

// MNIST_Image represents an image inside a MNIST dataset.
class MNIST_Image
{
private:
    uint indx_dataset; // The index of the image inside the MNIST dataset.
    IMAGE_DATA data;   // An array containing the pixel values of the image.
    double distance;   // The distance is used in various algorithms.
    int id;            // Unique Identifier of the image.

public:
    // Create a new instance of MNIST_Image.
    MNIST_Image() : indx_dataset(0), data{}, distance(pow(2, 32) - 5), id(-1)
    {
    }

    // Create a new instance of MNIST_Image.
    MNIST_Image(uint indx, IMAGE_DATA data) : indx_dataset(indx), data(data), distance(pow(2, 32) - 5), id(-1)
    {
    }

    // Get the index of the image inside the MNIST dataset.
    uint GetIndex()
    {
        return indx_dataset;
    }

    // Get the array containing the pixel values of the image.
    IMAGE_DATA GetImageData()
    {
        return data;
    }

    // Get the distance.
    double GetDist()
    {
        return distance;
    }

    // Set the distance.
    void SetDist(double _distance)
    {
        distance = _distance;
    }

    // Get the id.
    int GetId()
    {
        return id;
    }

    // Set the id.
    void SetId(int _id)
    {
        id = _id;
    }

    // Function that prints the image in the command line.
    string Print()
    {
        stringstream result;

        result << "++++++++++++++++++++++++++++++" << endl;
        for (int32_t i = 27; i >= 0; i--)
        {
            result << "+";
            for (int32_t j = 27; j >= 0; j--)
            {
                int pixelValue = data[i * 28 + j];
                char displayChar = '#';

                // Use ' ' for white and '#' for black based on the pixel value
                if (pixelValue < 128)
                {
                    displayChar = ' '; // Black
                }

                result << displayChar;
            }
            result << "+";
            result << endl;
        }

        result << "++++++++++++++++++++++++++++++";

        return result.str();
    };
};

// MNIST_ImageComparator is used in sets for sorting purposes.
struct MNIST_ImageComparator
{
    bool operator()(const MNIST_Image &a, const MNIST_Image &b) const
    {
        MNIST_Image first_image = a;
        MNIST_Image second_image = b;
        return first_image.GetDist() < second_image.GetDist();
    }
};

// MNIST contains the required functionality for reading MNIST dataset files.
class MNIST
{
private:
    string file_path;           // The MNIST file path.
    uint32_t magic_number;      // The MNIST magic number.
    uint32_t no_images;         // The total number of MNIST images.
    uint32_t no_rows;           // The number of rows that the images have.
    uint32_t no_columns;        // The number of columns that the images have.
    vector<MNIST_Image> images; // Vector containing all the MNIST images.

    // Function that extracts a unsigned integer value at the given offset.
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

    // Function that extracts the MNIST image data at the given offset.
    IMAGE_DATA ExtractArrayFromBytes(const vector<uint8_t> &bytes, size_t offset)
    {
        if (bytes.size() < offset + 784)
        {
            throw runtime_error("Vector does not contain enough bytes to extract the specified size.");
        }

        IMAGE_DATA data;

        for (size_t i = 0; i < 784; i++)
        {
            data[783 - i] = (double)bytes[offset + i];
        }

        return data;
    }

    // Function that read a MNIST file and populates the class instance.
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
    // Create a new instance of MNIST.
    MNIST(const string _file_path)
    {
        file_path = _file_path;
        vector<uint8_t> file_bytes = ReadFileAsBytes(file_path);

        magic_number = ExtractIntFromBytes(file_bytes, 0);
        no_images = ExtractIntFromBytes(file_bytes, 4);
        no_rows = ExtractIntFromBytes(file_bytes, 8);
        no_columns = ExtractIntFromBytes(file_bytes, 12);

        for (size_t i = 0; i < no_images; i++)
        {
            images.push_back(MNIST_Image((uint)i, ExtractArrayFromBytes(file_bytes, 16 + i * 784)));
        }
    };

    // Create a new instance of MNIST.
    MNIST(){};

    // Get the MNIST file path.
    string GetFilePath() { return file_path; }

    // Get the MNIST magic number.
    uint32_t GetMagicNumber() { return magic_number; }

    // Get the MNIST total of images.
    uint32_t GetImagesCount() { return no_images; }

    // Get the number of rows of the MNIST images.
    uint32_t GetRowsCount() { return no_rows; }

    // Get the number of columns of the MNIST images.
    uint32_t GetColumnsCount() { return no_columns; }

    // Get the MNIST's images.
    vector<MNIST_Image> GetImages() { return images; }

    /* Print the MNINST metadata. */
    void PrintMetadata()
    {
        cout << "MNIST File:        " << GetFilePath() << endl;
        cout << "Magic Number:      " << GetMagicNumber() << endl;
        cout << "Number of Images:  " << GetImagesCount() << endl;
        cout << "Number of Rows:    " << GetRowsCount() << endl;
        cout << "Number of Columns: " << GetColumnsCount() << endl;
    }
};

#endif // MNIST_H