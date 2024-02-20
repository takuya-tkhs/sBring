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
vector<vector<double>> network::accu_weighted_adjacency_matrix;
vector<vector<double>> network::log_weighted_adjacency_matrix;
vector<vector<int>> network::adjacency_list;
vector<int> network::node_degrees;

vector<double> prior::origin_time_prior;
vector<double> prior::origin_node_prior;

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

    //initialize the accumulated weighted adjacency matrix.
    network::accu_weighted_adjacency_matrix = vector<vector<double>>(N, vector<double>(N, 0.0));
    for(int i = 0; i < N; i++){
        network::accu_weighted_adjacency_matrix.at(i).at(0) = network::weighted_adjacency_matrix.at(i).at(0);
        for(int j = 1; j < N; j++){
            network::accu_weighted_adjacency_matrix.at(i).at(j) =
                network::accu_weighted_adjacency_matrix.at(i).at(j-1) + network::weighted_adjacency_matrix.at(i).at(j);
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


void initialize_origin_time_prior(int t_min, int T){
    prior::origin_time_prior = vector<double>(T + 1, 0.0);
    double prior_value = 1.0 / (T - t_min + 1);
    for(int t = t_min; t < T + 1; t++){
        prior::origin_time_prior.at(t) = prior_value;
    }
}


void initialize_origin_node_prior(int N){
    double prior_value = 1.0 / N;
    prior::origin_node_prior = vector<double>(N, prior_value);
}