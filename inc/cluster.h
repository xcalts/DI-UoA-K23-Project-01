#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>

using namespace std;

class Cluster
{
private:
    int no_clusters;
    int no_hash_tables;
    int no_hash_functions;
    int no_max_hypercubes;
    int no_dim_hypercubes;
    int no_probes;

public:
    Cluster(string conf_file)
    {
    }
};

#endif // CLUSTER_H