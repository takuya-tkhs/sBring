#include<vector>
#include"global_variables.h"
using namespace std;

//definition of global variables.

int N, M, T;

vector<int> data::state_dataset;
vector<int> data::known_node_list;
int data::num_state_one;

vector<vector<int>> network::adjacency_matrix;
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
void initialize_network(const vector<vector<int>>& adj_matrix){
    network::adjacency_matrix = adj_matrix;
    network::adjacency_list.resize(N);
    network::node_degrees = vector<int>(N, 0);
    
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(adj_matrix.at(i).at(j) == 1){
                network::adjacency_list.at(i).push_back(j);
                network::node_degrees.at(i) += 1;
            }
        }
    }
}