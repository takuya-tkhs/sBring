#include<vector>
#include<unordered_map>
#include<queue>
#include<math.h>
#include"tree_set.h"
#include"global_variables.h"
using namespace std;

void TreeSet::create_treeset_from_G(const vector<vector<int>>& G){
    this->initialize(M);

    //process to cut out the tree set from the G matrix.

    unordered_map<int, int> descendant;  // key: node ID (0..M-1, node in the network).  value: lineage id (starting from 0).
    for(int i = 0; i < M; i++){
        //initialize the unorderd_map descendant.
        descendant[data::known_node_list.at(i)] = i;

        //fill out information about tree leaves.
        spatial_node.at(i) = data::known_node_list.at(i);
        time_from_present.at(i) = 0;
        num_descendant_taxa_with_state_one.at(i) = data::state_dataset.at(data::known_node_list.at(i));  //0 or 1.
    }

    int current_lineage_id = M - 1;
    for(int t = 0; t < T; t++){
        unordered_map<int, int> next_descendant;

        for(auto itr = descendant.begin(); itr != descendant.end(); ++itr){
            int learner = itr->first;  //node ID.
            int j = itr->second;  //lineage ID.
            
            int model = G.at(t).at(learner);  //node ID.

            if(next_descendant.find(model) == next_descendant.end()){  //no coalescence
                next_descendant[model] = j;
            }else{   //coalescence
                int j2 = next_descendant[model]; //ID of the lineage which was originally in the model node.
                current_lineage_id += 1;

                child1.at(current_lineage_id) = j;
                child2.at(current_lineage_id) = j2;
                parent.at(j) = current_lineage_id;
                parent.at(j2) = current_lineage_id;
                spatial_node.at(current_lineage_id) = model;
                time_from_present.at(current_lineage_id) = t + 1;  //the internal node appears one timestep before.
                branch_length_to_child1.at(current_lineage_id) = t + 1 - time_from_present.at(j);
                branch_length_to_child2.at(current_lineage_id) = t + 1 - time_from_present.at(j2);
                branch_length_to_parent.at(j) = t + 1 - time_from_present.at(j);  //identical to the length from focal node to child 1.
                branch_length_to_parent.at(j2) = t + 1 - time_from_present.at(j2);

                //compute the number of descendent taxa with state 1.
                num_descendant_taxa_with_state_one.at(current_lineage_id)
                = num_descendant_taxa_with_state_one.at(j) + num_descendant_taxa_with_state_one.at(j2);

                next_descendant[model] = current_lineage_id;
            }
        }

        descendant = next_descendant;
    }

    //process to find the main tree. (the tree which contains all the taxa with state 1.)
    for(int i = 2 * M - 2; i >= 0; i--){
        if(num_descendant_taxa_with_state_one.at(i) == data::num_state_one){
            main_tree_root = i;
            is_state_one_contained_in_single_tree = true;
            break;
        }
    }

    //if the leaves with state 1 belong to multiple trees (no main tree exists), abort the routine.
    if(!is_state_one_contained_in_single_tree){
        return;
    }

    //Specify nodes which belong to the main tree.(BFS starting from the root of the main tree.)
    queue<int> queue_of_nodes;
    queue_of_nodes.push(main_tree_root);
    while(!queue_of_nodes.empty()){
        int current_node = queue_of_nodes.front();
        queue_of_nodes.pop();
        is_main_tree.at(current_node) = true;
        if(child1.at(current_node) != -1){  //Note that if child1 exists (id != -1), child2 always exists as well.
            queue_of_nodes.push(child1.at(current_node));
            queue_of_nodes.push(child2.at(current_node));
        }
    }

    //Specify the LCA (least common ancestor) of the state 1's.
    int current_node = main_tree_root;
    while(true){
        if(child1.at(current_node) != -1 && num_descendant_taxa_with_state_one.at(child1.at(current_node)) == data::num_state_one){
            current_node = child1.at(current_node);
        }else if(child2.at(current_node) != -1 && num_descendant_taxa_with_state_one.at(child2.at(current_node)) == data::num_state_one){
            current_node = child2.at(current_node);
        }else{
            lca_state_one = current_node;
            break;
        }
    }

    return;
}


//Pruning function.
//Create a vector of size 2M-1, where i-th element shows the probability of getting data at descendants, given i-th node has state 1.
//The result is contained in the vector "prob_data_given_state_one" member variable of the TreeSet class.
void TreeSet::pruning(double loss_rate){
    for(int i = 0; i < M; i++){  //leaves.
        if(!is_main_tree.at(i)){
            continue;
        }

        prob_data_given_state_one.at(i) = (data::state_dataset.at(spatial_node.at(i)) == 1 ? 1.0 : 0.0);
    }

    for(int i = M; i < 2 * M - 1; i++){  //internal nodes and root.
        if(!is_main_tree.at(i)){
            continue;
        }

        //probability that the child has state 1 given the focal node has state 1.
        double child1_prob_remain_one = pow(1 - loss_rate, branch_length_to_child1.at(i));
        double child2_prob_remain_one = pow(1 - loss_rate, branch_length_to_child2.at(i));

        //conditional probabilities given the focal node has state 1.
        double child1_prob = child1_prob_remain_one * prob_data_given_state_one.at(child1.at(i));
        if(num_descendant_taxa_with_state_one.at(child1.at(i)) == 0){  //if the child does not have descendants with state 1.
            child1_prob += 1 - child1_prob_remain_one;
        }
        double child2_prob = child2_prob_remain_one * prob_data_given_state_one.at(child2.at(i));
        if(num_descendant_taxa_with_state_one.at(child2.at(i)) == 0){  //if the child does not have descendants with state 1.
            child2_prob += 1 - child2_prob_remain_one;
        }

        prob_data_given_state_one.at(i) = child1_prob * child2_prob;
    }
}


//Compute the tree likelihood P(D|T, loss_rate), where the origin time and space are integrated out.
void TreeSet::compute_likelihood(double loss_rate){
    pruning(loss_rate);  //Call the pruning function.

    tree_likelihood = 0.0;
    int current_node = lca_state_one;
    while(true){
        int parent_node = parent.at(current_node);
        int current_branch_length 
        = (current_node != main_tree_root ? branch_length_to_parent.at(current_node) : T + 1 - time_from_present.at(current_node));

        tree_likelihood += prob_data_given_state_one.at(current_node) 
        / loss_rate * (1 - pow(1 - loss_rate, current_branch_length)) / (T + 1) / N;

        if(current_node == main_tree_root){
            break;
        }else{
            current_node = parent_node;
        }
    }
}


void TreeSet::initialize(int num_taxa){
    child1 = vector<int>(2 * num_taxa - 1, -1);
    child2 = vector<int>(2 * num_taxa - 1, -1);
    parent = vector<int>(2 * num_taxa - 1, -1);
    branch_length_to_child1 = vector<int>(2 * num_taxa - 1, -1);
    branch_length_to_child2 = vector<int>(2 * num_taxa - 1, -1);
    branch_length_to_parent = vector<int>(2 * num_taxa - 1, -1);
    time_from_present = vector<int>(2 * num_taxa - 1, -1);
    spatial_node = vector<int>(2 * num_taxa - 1, -1);
    num_descendant_taxa_with_state_one = vector<int>(2 * num_taxa - 1, -1);

    //initialized with false.
    is_state_one_contained_in_single_tree = false;
    is_main_tree = vector<bool>(2 * num_taxa, false);

    main_tree_root = -1;
    lca_state_one = -1;

    //for pruning.
    prob_data_given_state_one = vector<double>(2 * num_taxa - 1, -1.0);
}