#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <vector>

/* Read a file as a vector of bytes. */
std::vector<uint8_t> ReadFileAsBytes(const std::string &);

#endif // FILE_H