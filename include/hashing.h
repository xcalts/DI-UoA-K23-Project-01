#ifndef HASHING_H
#define HASHING_H

#include <array>
#include <vector>
#include <cmath>
#include <cstdint>
#include <random>

using namespace std;

/* Helper functions */

vector<vector<array<double, 784>>> GetRandomProjections(int, int);

uint CalculateFinalHashCode(array<uint8_t, 784>, vector<array<double, 784>>, int, int, int);

double CalculateDistance(int, array<uint8_t, 784>, array<uint8_t, 784>);

#endif // HASHING_H