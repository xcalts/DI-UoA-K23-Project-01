#ifndef MEM_H
#define MEM_H

#include <array>
#include <iostream>
#include <vector>

uint32_t ExtractIntFromBytes(const std::vector<uint8_t> &, size_t);
std::array<uint8_t, 784> ExtractArrayFromBytes(const std::vector<uint8_t> &, size_t);

#endif // MEM_H