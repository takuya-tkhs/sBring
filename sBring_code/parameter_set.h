#ifndef INCLUDE_GUARD_PARAMETER_SET_H
#define INCLUDE_GUARD_PARAMETER_SET_H

#include<vector>
#include"tree_set.h"
using namespace std;

struct Parameter_Set{
    vector<vector<int>> G;
    double loss_rate;
    int origin_time;
    int origin_node;

    TreeSet trees;

    double log_G_prior;  //P(G) (logarithm)
    double log_likelihood;  //P(Y|G, loss_rate) (logarithm)
    double log_posterior;

    void initialize_G(const vector<vector<int>>& mat);
    void initialize_G_by_shortest_path(int tentative_origin);
    void initialize_log_G_prior();
    void sample_origin(double rand_unif);
    void output_parameters(int sample_id);
};

#endif