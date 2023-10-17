#ifndef MEM_H
#define MEM_H

#include <array>
#include <iostream>
#include <vector>

using namespace std;

uint32_t ExtractIntFromBytes(const vector<uint8_t> &, size_t);
array<uint8_t, 784> ExtractArrayFromBytes(const vector<uint8_t> &, size_t);

#endif // MEM_H