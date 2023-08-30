#include<vector>
#include<math.h>
#include<iostream>
#include"global_variables.h"
#include"parameter_set.h"
using namespace std;

void Parameter_Set::initialize_G(const vector<vector<int>>& mat){
    G = mat;
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
    << "   Log Posterior: "  << to_string(log_posterior)  << endl;  
}