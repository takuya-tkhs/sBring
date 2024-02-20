#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include"global_variables.h"
#include"csv_input.h"
#include"MCMC.h"
#include"tree_set.h"  //for debugging.
using namespace std;

int main(){
    string data_file_name, network_file_name;
    cout << "Input the name of the file containing the state data." << endl;
    cin >> data_file_name;
    cout << "Input the name of the file containing the network data." << endl;
    cin >> network_file_name;

    initialize_dataset(input_from_csv_int(data_file_name, 1).at(0));
    initialize_network(input_from_csv_double(network_file_name, 1));

    int min_origin_time;
    cout << "Input the minimum and maximum extreme of the origin time." << endl;
    cin >> min_origin_time >> T;
    initialize_origin_time_prior(min_origin_time, T);
    initialize_origin_node_prior(N);

    cout << "Input the values of num_sample, length_burnin, and length_interval in this order." << endl;
    cin >> num_sample >> length_burnin >> length_interval;

    num_iteration = num_sample * length_interval + length_burnin;

    int seed;
    cout << "Input the seed value." << endl;
    cin >> seed;

    string result_file_name;
    cout << "Input the name of the result file (Do not include extension)." << endl;
    cin >> result_file_name;

    vector<Parameter_Set> result_samples = mcmc(seed);

    //output the result.
    ofstream result(result_file_name + ".csv");
    result << "#,loss_rate,origin_time,origin_node_id" << endl;
    for(int i = 0; i < num_sample; i++){
        Parameter_Set tmp = result_samples.at(i);
        result << i << "," << tmp.loss_rate << "," << tmp.origin_time << "," << tmp.origin_node << endl;
    }

    return(0);
}