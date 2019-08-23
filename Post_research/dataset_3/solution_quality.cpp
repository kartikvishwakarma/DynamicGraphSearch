#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <climits>
#include <utility>

using namespace std;

typedef pair<int,int>pr;
typedef vector<int> d_vect;
typedef map<int, d_vect> u_map;
typedef map<int, u_map> data_t;
typedef map<int, float > m_map;
typedef map <int, m_map> i_map;
typedef map<int, pr> _path_index;
typedef map<int, _path_index> _dep_index;

_dep_index pair_info;

data_t data, f_data;
i_map deviation;

const int inf = INT_MAX;
const string f_dir = "./SP_Exectime_10/";
//const string t_dir = "./Exectime_10_1_impromptus_30/";
const string t_dir = "./Exectime_10_1_impromptus_0/";

void f_readfile(string file);
void t_readfile(string file);
void show_data(data_t );
void show_deviation();
void get_deviation();
void disp();

fstream& GotoLine(fstream& file, unsigned int num){
    file.seekg(ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(numeric_limits<streamsize>::max(),'\n');
    }
    return file;
}

int dep_times[] = {0,450,540};
int path_lengths[] = {120,140,160,180,200,220,240};

int main(){
	string f_file = f_dir+"sp_tag_result.txt";
	//string t_file = "bi_direct_search_out.txt";
	string t_file = t_dir+"1st_impt10_0.txt";
	f_readfile(f_file);
		
	t_readfile(t_file);
	//show_data(data);
	
	get_deviation();
	show_deviation();
	//disp();
	return 0;
}

void disp(){
	for(auto d_time : dep_times){
		for(auto len : path_lengths){
			for(int i=0; i<10; i++){
				printf("d_time: %d path_length: %d ford: %d trace: %d\n",d_time, len, f_data[d_time][len][i], data[d_time][len][i]);			
			}
		printf("------------------------------------------------------------\n");
		}	
	}
}

void get_deviation(){
	for(auto &u_m : f_data){
		for(auto &i_m : u_m.second){
			int sum = 0;
			if(i_m.first>0){
				vector<int > trace_vector = data[u_m.first][i_m.first];
			//	if(u_m.first == 450  && i_m.first == 100)
					//printf("f:%d t:%d %ld %ld\n",u_m.first,i_m.first, trace_vector.size(), i_m.second.size());			
						
				for(int k=0; k<10; k++){
					//printf("k: %d %d %d\n",k, trace_vector[k], i_m.second[k]);
					sum += (trace_vector[k] - i_m.second[k]);
				}
				//deviation[u_m.first][i_m.first] = float(sum);
				deviation[u_m.first][i_m.first] = float(sum)/10.0f;
			}
		}
	}
}



void show_data(data_t data){
	for(auto &u_m : data){

		for(auto &i_m : u_m.second){
			cout<<i_m.second.size()  << "  d_time: " << u_m.first <<  "  path_len: "<< i_m.first << " | ";
			for(auto p : i_m.second)
				cout << p << ' ';
			cout << '\n';
		}
		
	}
}

void show_deviation(){
	for(auto &d : deviation){
		for(auto &p : d.second){
			cout << "d_time: " << d.first << " path_len: " 
				 << p.first << " dev.: " << p.second << '\n';
		}
	cout << "\n\n";
	}
}

void f_readfile(string file){	
	string line;
	int d_time, path_len, a_time;
	int closed_node;
	fstream infile(file);

	if(!infile.is_open()){
		printf("Error in open file: %s\n", file.c_str());
		_Exit(123);
	}
	
	//GotoLine(infile,4);

	while(infile.good()){
		getline(infile, line, '\n');
		//cout << line << '\n';
		stringstream ss(line);
		//printf("%d %d %d %d\n", d_time, path_len, a_time, closed_node);
		ss >> d_time >> path_len >> a_time >> closed_node;
		f_data[d_time][path_len].push_back(a_time);
	}
} 

void t_readfile(string file){
	fstream infile(file);

	if(!infile.is_open()){
		printf("Error in open file: %s\n", file.c_str());
		_Exit(123);
	}

	//GotoLine(infile, 11);
	string line, attr, tmp;
	//int attr;
	while(infile.good()){
		getline(infile, line, '\n');
		//cout << line << '\n';
		
		if( line.length() ){
			stringstream ss(line);
			int col = 0;
			int src, dest;
			int d_time, a_time, path_len;
			d_time = (a_time = (path_len = inf) );
			while(ss){
				ss >> attr;
				if(col == 1)
					src = stoi(attr);
				if(col == 7)
					dest = stoi(attr);
				if(col == 4){
					//cout << attr << ' ';
					d_time = stoi(attr);
				}
				if(col == 9){
					//cout << attr << ' ';
					a_time = stoi(attr);
				}
				if(col == 17){
					stringstream ll(attr);
					getline(ll, tmp, ')');
					//cout << tmp << ' ';
					path_len = stoi(tmp);
				}
				col += 1;
			}
			
			//usleep(1000000);
			data[d_time][path_len].push_back(a_time);
			//pair_info[d_time][path_len] = {src, dest};
		}
	
	}
}
