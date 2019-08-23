#include "header.h"
#include "search.h"


//const int no_src_dest_pair = 15;
const int no_src_dest_pair = 10;

const string data_path = "timeseries_50000.txt";

//const string In_dir = "./Querypoints_15/";
const string In_dir = "../other/Querypoints_10/";
//const string In_dir = "./Datafile/";

//const string Out_dir = "./Exectime_15/";
const string Out_dir = "./SP_Exectime_10/";
//const string Out_dir = "./Searchfile/";

void readcase(string dir_path, Object& obj);

int main(){
	
	Readfile(data_path);
	Create_ealiest_arrival();
	readcase("./", obj);
	//obj.parallel_SP_TAG(224111, 189454, 0);
	return 0; 
}


void readcase(string dir_path, Object& obj){
	int dep_times[] = {0,450,540} ;//{0,540,600,1140,1200};
	string patt = "(50000_src_dest_)(.*)";
	string file;
	DIR *dp, *dp_out;
	struct dirent *ep;

	dp = opendir(In_dir.c_str());


	if(dp == NULL){
		perror("Failed to open Directory.");
		_Exit(123);
	}

	if(opendir(Out_dir.c_str()) == NULL) {
		printf("Creating search Directory\n");
		const int dir_err = mkdir(Out_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
		    printf("Error creating directory: %s\n", Out_dir.c_str());
		    exit(1);
		}

	}
	
	ofstream out_trace;
	string name = "sp_tag_result.txt";
	
	out_trace.open(Out_dir+name);
	if(!out_trace.is_open()){
		printf("Error in open file: %s\n", name.c_str());
		_Exit(123);
	}

	while(ep = readdir(dp)){
		file = string(ep->d_name);
		if(regex_match(file, regex(patt)) ){
			//cout << file << endl;

			ifstream infile;
			infile.open(In_dir+file);
			if(!infile.is_open()){
				printf("Error in open file: %s\n", ep->d_name);
				_Exit(102);
			}

			ofstream outfile;
			
			string line;
			int src, dest, len;

			stringstream ptr(file);
			string dp_time, other, p_len;
			for(int i=0; i<3; i++)			
				getline(ptr, other, '_');
			getline(ptr, dp_time, '_');
			getline(ptr, p_len, '.');
			//cout<< file << '\t' << dp_time << ' ' << p_len << endl;

			//int time = stoi(dp_time);

			while(infile.good()){
				getline(infile, line, '\n');
				stringstream ss(line);

				ss >> src >> dest >> len;
				for(int k=0; k<3 && len>0; k++){
					int d_time = dep_times[k];
					string out = Out_dir + "sp_" + to_string(d_time) + "_" + file;
					outfile.open(out, ios_base::app);

				
					
					if(!outfile.is_open()){
						printf("Error in open file: %s\n", out.c_str());
						_Exit(101);
					}
					auto t1 = steady_clock::now();
					obj.Forward_search(src, dest, d_time, len, out_trace);
					//obj.parallel_SP_TAG(src, dest, d_time, len);	
					//dist_SP_TAG(src, dest, d_time, 10,len);					
					auto t2 = steady_clock::now();
					auto duration = duration_cast<microseconds>(t2-t1).count();
					//cout << src << ' ' << dest << ' '<< duration << '\n';
			
					outfile << src << ' ' << dest << ' ' << duration << '\n';	
					outfile.close();
				}
			}
			infile.close();
			
		}
	}
}


