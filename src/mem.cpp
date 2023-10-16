#include <array>
#include <iostream>
#include <vector>

#include "mem.h"

using namespace std;

uint32_t ExtractIntFromBytes(const std::vector<uint8_t> &bytes, size_t offset)
{
    if (bytes.size() < offset + 4)
    {
        throw std::runtime_error("Vector does not contain enough bytes to convert to a 32-bit integer at the specified offset.");
    }

    uint32_t integerVal = 0;

    // Extract 4 bytes and convert to a 32-bit integer in a reverse byte order
    for (int i = 0; i < 4; i++)
    {
        integerVal |= static_cast<uint32_t>(bytes[offset + 3 - i]) << (i * 8);
    }

    return integerVal;
}

std::array<uint8_t, 784> ExtractArrayFromBytes(const std::vector<uint8_t> &bytes, size_t offset)
{
    if (bytes.size() < offset + 784)
    {
        throw std::runtime_error("Vector does not contain enough bytes to extract the specified size.");
    }

    std::array<uint8_t, 784> data;

    for (size_t i = 0; i < 784; i++)
    {
        data[783 - i] = bytes[offset + i];
    }

    return data;
}