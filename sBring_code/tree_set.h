#ifndef INCLUDE_GUARD_TREE_SET_H
#define INCLUDE_GUARD_TREE_SET_H

#include<vector>
using namespace std;

//struct which contains (potentially) multiple trees cut out from the G matrix.
struct TreeSet{
    //Here are vectors which contain information about each tree node (M leaves, M - 2 internal nodes, 1 root).
    //However, if multiple trees are cut out from G, the number of nodes will decrease and last few elements will not be used.

    vector<int> child1;  //index of the child node 1.
    vector<int> child2;  //index of the child node 2.
    vector<int> parent;  //index of the parent node.
    vector<int> branch_length_to_child1;  //length of the branch between the focal node and the child node 1.
    vector<int> branch_length_to_child2;  //length of the branch between the focal node and the child node 2.
    vector<int> branch_length_to_parent;  //length of the branch between the focal node and the parent node.
    vector<int> time_from_present;  //time from the present.
    vector<int> spatial_node;  //node (in the network) to which the focal node belongs.
    vector<int> num_descendant_taxa_with_state_one;  //number of taxa with state 1 with the focal node as an ancestor.

    bool is_state_one_contained_in_single_tree;  //true if all the taxa with state 1 are contained in a single tree (called main tree).

    //The following variables are not applicable unless the main tree exists.

    vector<bool> is_main_tree;  //true if the node belongs to the main tree. (i.e. the tree which contains all the taxa with state 1)
    int main_tree_root;  //node ID assigned to the root of the main tree.
    int lca_state_one;  //node ID of the least common ancestor of state 1's.
    vector<int> spatial_node_main_lineage; //possible origin node as a function of time [length: T + 1].
    vector<double> prob_data_given_state_one;  //result of pruning. probability of data in descendant leaves given state 1 at focal node.
    double tree_likelihood;  //P(data|tree, loss_rate)

    void initialize(int num_taxa);  //initialization.

    //create a TreeSet from the G matrix.
    void create_treeset_from_G(const vector<vector<int>>& G);

    //pruning & computation of tree likelihood.
    void pruning(double loss_rate);
    void compute_likelihood(double loss_rate);
};

#endif