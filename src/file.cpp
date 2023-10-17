#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "file.h"

using namespace std;

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