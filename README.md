# project-one-DI352

1η εργασία - Αναζήτηση και συσταδοποίηση διανυσμάτων στη C/C++

## Instructions

```sh
$ make debug
$ ./bin/lsh -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -o results.txt --hash-function 15 --hash-tables 10 --num-nearest 5 -R 0
$ ./bin/cube -i data/train-images.idx3-ubyte -q data/t10k-images.idx3-ubyte -o results.txt -M 100 -probes 10 -k 14
$ ./bin/cluster -i data/train-images.idx3-ubyte
```
