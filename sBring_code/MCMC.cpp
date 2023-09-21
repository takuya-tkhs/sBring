#include<vector>
#include<random>
#include<math.h>
#include<iostream>
#include"global_variables.h"
#include"parameter_set.h"
#include"tree_set.h"
#include"mcmc.h"
using namespace std;

vector<Parameter_Set> mcmc(int seed){
    mt19937 random(seed);
    uniform_real_distribution<> rand_real(0, 1);
    uniform_real_distribution<> rand_change_loss_rate(- change_loss_rate, change_loss_rate);
    uniform_int_distribution<> rand_int(0, pow(10, 9));

    Parameter_Set last_parameter;
    vector<Parameter_Set> sampled_parameters(num_sample);
    int current_sample_id = 0;
    last_parameter.initialize_G_by_shortest_path(rand_int(random) % N);
    last_parameter.initialize_log_G_prior();

    last_parameter.loss_rate = (min_loss_rate + max_loss_rate) / 2.0;

    //set the initial posterior to - Inf, so that the first step of the MH-algorithm is always accepted.
    last_parameter.log_posterior = - INFINITY;

    for(long long itr = 0; itr < num_iteration; itr++){
        //copy the last parameter set.
        Parameter_Set proposed_parameter = last_parameter;

        //change loss rate (just slightly).
        proposed_parameter.loss_rate = last_parameter.loss_rate + rand_change_loss_rate(random);
        if(proposed_parameter.loss_rate < min_loss_rate || proposed_parameter.loss_rate > max_loss_rate){
            proposed_parameter.loss_rate = last_parameter.loss_rate;
        }

        //change G.
        for(int i = 0; i < num_G_change; i++){
            int change_time = rand_int(random) % T;
            int change_node = rand_int(random) % N;
            int tmp = rand_int(random) % network::node_degrees.at(change_node);

            int new_model = network::adjacency_list.at(change_node).at(tmp);
            int old_model = proposed_parameter.G.at(change_time).at(change_node);

            //update prior P(G).
            proposed_parameter.log_G_prior += network::log_weighted_adjacency_matrix.at(change_node).at(new_model);
            proposed_parameter.log_G_prior -= network::log_weighted_adjacency_matrix.at(change_node).at(old_model);

            //assign the new element to the G matrix.
            proposed_parameter.G.at(change_time).at(change_node) = new_model;
        }

        //cut out trees.
        proposed_parameter.trees.create_treeset_from_G(proposed_parameter.G);

        //see if the main tree exists (i.e. if 1's are included in a single tree).
        if(proposed_parameter.trees.is_state_one_contained_in_single_tree){
            //compute the tree likelihood.
            proposed_parameter.trees.compute_likelihood(proposed_parameter.loss_rate);
            //recalculate the posterior probability.
            proposed_parameter.log_likelihood = log10(proposed_parameter.trees.tree_likelihood);
        }else{  //if the main tree does not exist...
            proposed_parameter.log_likelihood = - INFINITY;
        }

        proposed_parameter.log_posterior = proposed_parameter.log_G_prior + proposed_parameter.log_likelihood;

        //acceptance or rejection?
        double acceptance_ratio = pow(10, proposed_parameter.log_posterior - last_parameter.log_posterior);
        if(rand_real(random) < acceptance_ratio){
            last_parameter = proposed_parameter;
        }

        if(itr >= length_burnin && itr % length_interval == 0){
            last_parameter.sample_origin(rand_real(random));  //sample origin_time & origin_node
            sampled_parameters.at(current_sample_id) = last_parameter;
            last_parameter.output_parameters(current_sample_id);

            current_sample_id += 1;
        }
    }

    return(sampled_parameters);
}