#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "file.h"

using namespace std;

std::vector<uint8_t> ReadFileAsBytes(const std::string &file_path)
{
    std::vector<uint8_t> file_bytes;

    // Open the file in binary mode
    std::ifstream file(file_path, std::ios::binary);

    // Check if the file is open
    if (!file.is_open())
    {
        std::cerr << "Failed to open the file: " << file_path << std::endl;
        return file_bytes; // Return an empty vector in case of an error
    }

    // Read the file into a vector of bytes
    file.seekg(0, std::ios::end); // Move to the end of the file to determine its size
    size_t file_size = file.tellg();
    file_bytes.resize(file_size);
    file.seekg(0, std::ios::beg); // Move back to the beginning of the file
    file.read(reinterpret_cast<char *>(file_bytes.data()), file_size);

    // Check if the file was read successfully
    if (!file)
    {
        std::cerr << "Failed to read the file: " << file_path << std::endl;
        file_bytes.clear(); // Clear the vector in case of an error
    }

    // Close the file
    file.close();

    return file_bytes;
}