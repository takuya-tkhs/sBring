#include<vector>
#include<math.h>
#include<queue>
#include<iostream>
#include"global_variables.h"
#include"parameter_set.h"
using namespace std;

void Parameter_Set::initialize_G(const vector<vector<int>>& mat){
    G = mat;
}


//This function initializes G using the shortest path algorithm (BFS).
//Given a tentative origin, each node i learns from a node j such that j is in the shortest path from i to the tentative origin.
void Parameter_Set::initialize_G_by_shortest_path(int tentative_origin){
    //i-th element is i's neighbour which is in the shortest path from i to the tentative origin.
    vector<int> shortest_path_neighbour(N, -1);
    shortest_path_neighbour.at(tentative_origin) = tentative_origin;

    queue<int> queue_of_nodes;
    queue_of_nodes.push(tentative_origin);

    //Breadth first search
    while(!queue_of_nodes.empty()){
        int i = queue_of_nodes.front();
        queue_of_nodes.pop();
        for(auto & j : network::adjacency_list.at(i)){
            if(shortest_path_neighbour.at(j) == -1){  //not visited yet.
                shortest_path_neighbour.at(j) = i;
                queue_of_nodes.push(j);
            }
        }
    }

    //initialize G.
    G = vector<vector<int>>(T, vector<int>(N, -1));
    for(int t = 0; t < T; t++){
        G.at(t) = shortest_path_neighbour;
    }
}


//This function initializes P(G) based on the initial value of G.
//This function is used at the first step of the inference.
void Parameter_Set::initialize_log_G_prior(){
    log_G_prior = 0.0;
    for(int t = 0; t < T; t++){
        for(int i = 0; i < N; i++){
            int j = G.at(t).at(i);
            log_G_prior += network::log_weighted_adjacency_matrix.at(i).at(j);
        }
    }
}


//This function draw a sample of origin_time and origin_node.
//The tree must be cut out, and the likelihood must be computed before this function is called.
//rand_unif: random value (double) from 0 to 1.
void Parameter_Set::sample_origin(double rand_unif){
    //Trace the lineage.
    vector<int> spatial_node_main_lineage(T + 1, -1);  //This vector records to which node the main lineage belonged at each timestep t.
    int lineage_node = trees.spatial_node.at(trees.lca_state_one);
    int lineage_time = trees.time_from_present.at(trees.lca_state_one);
    while(true){
        spatial_node_main_lineage.at(lineage_time) = lineage_node;

        if(lineage_time == T){
            break;
        }else{
            lineage_node = G.at(lineage_time).at(lineage_node);
            lineage_time += 1;
        }
    }

    //computation of likelihood.

    //P(Y|tree, loss_rate, origin_time, origin_space)P(origin_time)P(origin_space). Index: time 0..T (time before present)
    vector<double> likelihood_by_origin_time(T + 1, 0.0);

    int current_node = trees.lca_state_one;
    while(true){
        int current_node_time = trees.time_from_present.at(current_node);
        int parent_node = trees.parent.at(current_node);
        int parent_node_time = (current_node != trees.main_tree_root ? trees.time_from_present.at(parent_node) : T + 1);

        for(int t = current_node_time; t < parent_node_time; t++){
            likelihood_by_origin_time.at(t) 
            = trees.prob_data_given_state_one.at(current_node) * pow(1 - loss_rate, t - current_node_time) / (T + 1) / N;
        }

        if(current_node == trees.main_tree_root){
            break;
        }else{
            current_node = parent_node;
        }
    }


    //sample origin time and node.
    double cummulative_normalized_likelihood_by_origin_time = 0.0;
    for(int t = 0; t < T + 1; t++){
        cummulative_normalized_likelihood_by_origin_time += likelihood_by_origin_time.at(t) / trees.tree_likelihood;
        if(rand_unif < cummulative_normalized_likelihood_by_origin_time){  //sampling.
            origin_time = t;
            origin_node = spatial_node_main_lineage.at(t);
            break;
        }
    }
}


//output parameters on the screen.
void Parameter_Set::output_parameters(int sample_id){
    cout << "Sample ID: "  << to_string(sample_id)
    << "   Loss Rate: " << to_string(loss_rate)
    << "   Origin Time: "  << to_string(origin_time)
    << "   Origin Node ID: "  << to_string(origin_node)
    << "   Log Prior: "  << to_string(log_G_prior)
    << "   Log Likelihood: "  << to_string(log_likelihood)
    << "   Log Posterior: "  << to_string(log_posterior)  << endl;
}


/////Side functions

//samples the node from which the learner copies the state.
//learner_id : 0 ~ N-1
//p: random variable sampled from Uniform(0, 1)
int learn(int learner_id, double p){
    int model_l = 0;
    int model_r = N - 1;

    while(model_l != model_r){
        int model_c = (model_l + model_r) / 2;
        if(p < network::accu_weighted_adjacency_matrix.at(learner_id).at(model_c)){
            model_r = model_c;
        }else{
            model_l = model_c + 1;
        }
    }

    return(model_l);
}