#ifndef INCLUDE_GUARD_GLOBAL_VARIABLES_H
#define INCLUDE_GUARD_GLOBAL_VARIABLES_H

#include<vector>
using namespace std;

//N: number of nodes.   M: number of nodes where the present state is known.  T: upper limit of the time of origin.
extern int N, M, T;

namespace data{
    extern vector<int> state_dataset;  //size N. each element represents the state of each node. 
    extern vector<int> known_node_list;  //size M. list of node indeces whose state is known. (number of taxa in the set of trees)
    extern int num_state_one; //number of nodes with state 1.
}

namespace network{
    extern vector<vector<double>> weighted_adjacency_matrix;  //weighted_adjacency_matrix.
    extern vector<vector<double>> accu_weighted_adjacency_matrix;  //accumulated weighted_adjacency_matrix.
    extern vector<vector<double>> log_weighted_adjacency_matrix;  //logarithm (base 10).
    extern vector<vector<int>> adjacency_list;  //self loop not included in the list.
    extern vector<int> node_degrees;  //self loop not counted.
}

namespace prior{
    extern vector<double> origin_time_prior;  //length T + 1 (including origin time 0)
    extern vector<double> origin_node_prior;  //length N
}

extern double min_loss_rate, max_loss_rate, change_loss_rate;
extern int num_G_change;

extern long long num_sample, length_interval, length_burnin, num_iteration;


//functions to initialize global variables.
void initialize_dataset(const vector<int>& dataset);
void initialize_network(const vector<vector<double>>& adj_matrix);
void initialize_origin_time_prior(int t_min, int T);
void initialize_origin_node_prior(int N);

#endif