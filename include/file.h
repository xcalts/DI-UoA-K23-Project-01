#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <vector>

using namespace std;

/* Read a file as a vector of bytes. */
vector<uint8_t> ReadFileAsBytes(const string &);

#endif // FILE_H