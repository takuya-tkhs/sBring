#ifndef INCLUDE_GUARD_CSV_INPUT_H
#define INCLUDE_GUARD_CSV_INPUT_H

#include<vector>
#include<string>
using namespace std;

vector<string> split(string str, string sep);
vector<vector<string>> input_from_csv(string file_name, int num_header);
vector<vector<int>> input_from_csv_int(string file_name, int num_header);
vector<vector<double>> input_from_csv_double(string file_name, int num_header);

#endif