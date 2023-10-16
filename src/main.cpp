#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "file.h"
#include "mnist.h"

using namespace std;

int main()
{
    std::string file_path = "data/t10k-images.idx3-ubyte";

    // initialize mnist
    MNIST mnist = MNIST(file_path);

    mnist.Print();

    return 0;
}