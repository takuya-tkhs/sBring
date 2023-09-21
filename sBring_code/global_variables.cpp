#include<vector>
#include<math.h>
#include"global_variables.h"
using namespace std;

//definition of global variables.

int N, M, T;

vector<int> data::state_dataset;
vector<int> data::known_node_list;
int data::num_state_one;

vector<vector<double>> network::weighted_adjacency_matrix;
vector<vector<double>> network::log_weighted_adjacency_matrix;
vector<vector<int>> network::adjacency_list;
vector<int> network::node_degrees;

double min_loss_rate = 0.0, max_loss_rate = 1.0, change_loss_rate = 0.01;
int num_G_change = 100;

long long num_sample, length_interval, length_burnin, num_iteration;


//The followings are functions to initialize global variables.


//Initialize N, M, and other variables in "data" namespace.
//dataset: vector with elements 0, 1, or -1.
void initialize_dataset(const vector<int>& dataset){
    N = dataset.size();
    data::state_dataset.resize(N);

    for(int i = 0; i < N; i++){
        data::state_dataset.at(i) = dataset.at(i);
        if(dataset.at(i) != -1){
            data::known_node_list.push_back(i);
        }
        if(dataset.at(i) == 1){
            data::num_state_one += 1;
        }
    }

    M = data::known_node_list.size();
}


//This function should be called after initializing N, M, and other variables in "data" namespace.
//adj_matrix: weighted adjacency matrix.
void initialize_network(const vector<vector<double>>& adj_matrix){
    //normalization of the weighted adjacency matrix.
    network::weighted_adjacency_matrix = vector<vector<double>>(N, vector<double>(N, 0.0));
    for(int i = 0; i < N; i++){
        double sum_weight = 0.0;
        for(int j = 0; j < N; j++){
            sum_weight += adj_matrix.at(i).at(j);
        }
        for(int j = 0; j < N; j++){
            network::weighted_adjacency_matrix.at(i).at(j) = adj_matrix.at(i).at(j) / sum_weight;
        }
    }

    //initialize the logarithm of the weighted adjacency matrix.
    network::log_weighted_adjacency_matrix = vector<vector<double>>(N, vector<double>(N, 0.0));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(network::weighted_adjacency_matrix.at(i).at(j) > 0.0){
                network::log_weighted_adjacency_matrix.at(i).at(j) = log10(network::weighted_adjacency_matrix.at(i).at(j));
            }else{  //we give non-zero probability even if the nodes are not neighbors.
                network::log_weighted_adjacency_matrix.at(i).at(j) = - 20.0;
            }
        }
    }

    network::adjacency_list.resize(N);
    network::node_degrees = vector<int>(N, 0);
    
    //initialize the adjacency list. note that the node itself is also regarded as a neighbour (self-loop).
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(network::weighted_adjacency_matrix.at(i).at(j) > 0.0){
                network::adjacency_list.at(i).push_back(j);
                network::node_degrees.at(i) += 1;
            }
        }
    }
}