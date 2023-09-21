#include"csv_input.h"
#include<vector>
#include<string>
#include<fstream>
using namespace std;

vector<string> split(string str, string sep){
	int char_start = 0;

	vector<string> result;
	bool finish = false;

	while(!finish){
		long long id_sep = char_start + str.substr(char_start, str.size() - char_start).find_first_of(sep);
		if(id_sep == char_start + string::npos){
			id_sep = str.size();
			finish = true;
		}

		result.push_back(str.substr(char_start, id_sep - char_start));

		char_start = id_sep + 1;
	}

	return(result);
}


//This function creates a matrix (vector<vector>) from a CSV file.
//num_header: number of lines to be ignored.
vector<vector<string>> input_from_csv(string file_name, int num_header){
	vector<vector<string>> result;
	ifstream reading_file;

	reading_file.open(file_name, ios::in);
	string reading_line_buffer;

	int current_line_num = 0;

	while(getline(reading_file, reading_line_buffer)){
		if(reading_line_buffer.size() == 0){
			break;
		}
		vector<string> split_line;
		split_line = split(reading_line_buffer, ",");
		if(current_line_num >= num_header){
			result.push_back(split_line);
		}
		current_line_num += 1;
	}

	return(result);
}


//input from a CSV file. Create a matrix of int.
vector<vector<int>> input_from_csv_int(string file_name, int num_header){
	vector<vector<string>> input_string_matrix = input_from_csv(file_name, num_header);
	int num_rows = input_string_matrix.size();

	vector<vector<int>> result(num_rows, vector<int>(0));
	for(int i = 0; i < num_rows; i++){
		int num_cols = input_string_matrix.at(i).size();
		result.at(i).resize(num_cols);
		for(int j = 0; j < num_cols; j++){
			result.at(i).at(j) = stoi(input_string_matrix.at(i).at(j));
		}
	}

	return(result);
}


//input from a CSV file. Create a matrix of double.
vector<vector<double>> input_from_csv_double(string file_name, int num_header){
	vector<vector<string>> input_string_matrix = input_from_csv(file_name, num_header);
	int num_rows = input_string_matrix.size();

	vector<vector<double>> result(num_rows, vector<double>(0));
	for(int i = 0; i < num_rows; i++){
		int num_cols = input_string_matrix.at(i).size();
		result.at(i).resize(num_cols);
		for(int j = 0; j < num_cols; j++){
			result.at(i).at(j) = stod(input_string_matrix.at(i).at(j));
		}
	}

	return(result);
}