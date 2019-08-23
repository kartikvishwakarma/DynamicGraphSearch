
/* g++ -fopenmp  -mcmodel=medium bidirectional.cpp -o bidirectional -std=c++11 -g 
   for 50,000 graph preprocessing time approx. 15 minutes.
   preprocessing include:
			-- loading lower and upper bound 
			-- graph creation
			-- earliest arrival graph
			-- reverse graph creation
*/

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <regex>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <omp.h>
#include <thread>

using namespace std;
using namespace std::chrono;

const int time_length = 10080;
const int inf = INT_MAX;
const int window_size = 40;
//const int vertex_size = 10000;
const int vertex_size = 50000;

const int no_src_dest_pair = 10;
const string data_dir = "../experiments/Urban_Network/testcases/";
//const string timeseries_path = data_dir + "timeseries_10000.txt";
const string timeseries_path = "timeseries_50000.txt";
//const string inputfile = data_dir + "edges_10000.csv";
const string inputfile = "Edges_50000.csv";
const string bound_dir = "../experiments/Urban_Network/testfiles/";

//const string In_dir = "./Datafile_10000/";
const string In_dir = "../other/Querypoints_10/";
//const string In_dir = "./Datafile/";

//const string Out_dir = "./Searchfile_10000/";
//const string Out_dir = "./Exectime_10/";
const string Out_dir = "./Exectime_10_1_impromptus_30/";
//const string Out_dir = "./Searchfile/";

int theta = 30;
static int Lower_bound[vertex_size][vertex_size];
//static int Upper_bound[vertex_size][vertex_size];

//const string vertex_file = bound_dir + "vertex_order.txt";
const string vertex_file = "../common/vertex_order_50000.txt";
//const string LBT_file = bound_dir + "lower_dist_matrix_10000.txt";
const string LBT_file = "../common/lower_dist_matrix_50000.txt";
//const string UBT_file = bound_dir + "upper_dist_matrix_10000.txt";
const string UBT_file = "upper_dist_matrix_50000.txt";

typedef int si;
typedef unordered_map<si,si> UMap;
typedef pair<int, int> pr;
typedef unordered_map<int, vector<int> > vect;
typedef unordered_map<int, multimap<int, int> > M_Map;
typedef unordered_map<int, vect> TAG_Graph;
typedef unordered_map<int, M_Map> R_TAG_Graph;
typedef unordered_map<int, pr > Edgeinfo;
typedef unordered_map<int, int> Map;
typedef unordered_map<int, bool> Mark;
typedef priority_queue<pr> R_Queue;
typedef unordered_map<int, char> Flag;
typedef priority_queue<pr, vector<pr>, greater<pr> > Queue;

TAG_Graph tag_graph, rev_tag_graph;
Edgeinfo edgeinfo;
map<int, map<int, int> > graph;
unordered_set<int> S;
vector<pr> gen;
UMap vertex;

Map dist[window_size+1];
Map parent[window_size+1];
Map least_flat_time[window_size+1];
Flag visited[window_size+1];
Flag is_flat[window_size+1];
unordered_map<int,bool> closed[window_size+1];

char node[] = {'S', 'A', 'C', 'B', 'D', 'E', 'F', 'G'};    // timeseries.txt
//char node[] = {'S', 'A', 'B', 'F', 'D'};				   // timeseries_1.txt
//char node[] = {'S', 'A', 'B', 'C', 'D', 'E', 'F', 'G'};  // timeseries_2.txt
//char node[] = {'A', 'B', 'C', 'D'};


bool f_stop = false;
bool first_impromptu = false;
int impromptu_count = 0;


void Readfile(string file);
void Create_reverse_graph(void);
void Create_ealiest_arrival(void);
void Insert(int edge, int src, int dest);
void Bidirection_search(int src, int dest, int d_time);
void Show_tag_graph(void);
void Show_reverse_graph(void);
void Bidirection_search(int src, int dest, int d_time, int LBT, int len, ofstream& out_trace);
void Get_vertex(string file);
void Read_bound_file(string file, char type);
void readcase(string dir);

class Object{
	Queue Q;

public:
	//Object();
	//Object(const Object& n);
	//Object& operator = (const Object &n);
	int Forward_search(int src, int dest, int d_time, int id);
	int Trace_search(int src, int dest, int d_time, int a_time, int id);
	//int Impromptu_rendezous(int id, int dest);
	int Impromptu_rendezous(int f_id, int t_id, int node, int dest);
	void Show_path(int dest, int id);
	void Show_r_path(int dest, int id);
	void Show_dist(int src);
	void Show_flat_times(void);

	Object(){ Q = Queue(); }	
	
	Object(const Object &n){ Q = Queue(n.Q); }

	/*Object& operator = (const Object& n){
		if(this != &n){
			dist = Map(n.dist);
			parent = Map(n.parent);
			least_flat_time = Map(n.least_flat_time);
			visited = Flag(n.visited);
			is_flat = Flag(n.is_flat);
			Q = Queue(n.Q);
		}
		return *this;
	}*/
};

Object obj;

int main()
{
	int src = 80763;//0;
	int dest = 1005;//7;
	int d_time = 0;
	int a_time = 280;//19;
	printf("Getting lower and upper bound\n");
	Get_vertex(vertex_file);
	Read_bound_file(LBT_file, 'l');
	//Read_bound_file(UBT_file, 'u');

	int LBT =  Lower_bound[vertex[src]][vertex[dest]];
	//int UBT =  Upper_bound[vertex[src]][vertex[dest]];
	//printf("LBT: %d\tUBT: %d\n", LBT, UBT);
	
	auto t1 = steady_clock::now();
	Readfile(timeseries_path);
	auto t2 = steady_clock::now();
	auto duration = duration_cast<milliseconds>(t2-t1).count();
	cout << "graph creation time: " << duration << '\n';

	
	t1 = steady_clock::now();
	Create_ealiest_arrival();
	t2 = steady_clock::now();
	duration = duration_cast<milliseconds>(t2-t1).count();
	cout << "earliest arrival creation time: " << duration << '\n';


	t1 = steady_clock::now();
	Create_reverse_graph();
	t2 = steady_clock::now();
	duration = duration_cast<milliseconds>(t2-t1).count();
	cout << "reverse graph creation time: " << duration << '\n';

	cout << "\n\n";
	
	readcase("./");
	
	return 0;
}

void readcase(string dir_path){
	int dep_times[] = {0,450,540};//{0,540,600,1140,1200};
	//string patt = "(10000_src_dest_)(.*)";
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
	string name = "1st_impt10_"+to_string(theta)+".txt";
	
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
			string d_time, other, p_len;
			for(int i=0; i<3; i++)			
				getline(ptr, other, '_');
			getline(ptr, d_time, '_');
			getline(ptr, p_len, '.');
			//cout<< file << '\t' << d_time << ' ' << p_len << endl;

			int time = stoi(d_time);

			while(infile.good()){
				getline(infile, line, '\n');
				stringstream ss(line);

				ss >> src >> dest >> len;
				int LBT = Lower_bound[vertex[src]][vertex[dest]];
				//int UBT = Upper_bound[vertex[src]][vertex[dest]];

				for(int k=0; k<3 && len>0; k++){
					int d_time = dep_times[k];
					//string out = Out_dir + "sp_" + file;
					//string out = Out_dir + "par-sp_" + file;
					string out = Out_dir + "bi-dict_" + to_string(d_time) + "_" + file;
			
					outfile.open(out, ios_base::app);

			

					if(!outfile.is_open()){
						printf("Error in open file: %s\n", out.c_str());
						_Exit(101);
					}
						//if(len>=200){
						
						auto t1 = steady_clock::now();
						//printf("searching for src: %d dest: %d d_time: %d len: %d\n",
						//src, dest, time, len);
				
						Bidirection_search(src, dest, d_time, LBT,len, out_trace);
						auto t2 = steady_clock::now();
						auto duration = duration_cast<microseconds>(t2-t1).count();
						//cout << src << ' ' << dest << ' '<< duration << '\n';
				
						outfile << src << ' ' << dest << ' ' << duration << '\n';
						outfile.close();						
						//cout << src << ' ' << dest << ' ' << len << '\n';
						//}
						//Ptr p = Ptr(src, dest, true);
					}
			}
			infile.close();
		}
	}
}

int Object::Impromptu_rendezous(int f_id, int t_id, int node, int dest){
	//printf("f_id: %d  t_id: %d\n", f_id, t_id);
	int f_time = dist[f_id][node];
	int t_time = dist[t_id][node];
	int arrival_time = dist[t_id][dest];

	if( is_flat[t_id][node] && (f_time >= least_flat_time[t_id][node]) && (f_time <= t_time) ) {
		//printf("f flatter case with lower time: %d  upper time: %d\n", least_Flat_time[i][p.first], t_time);
		for(int time = t_time; time >= least_flat_time[t_id][node]; time -= 1){
			if( (time == f_time)  ){
				//printf("\n\n(f) thread: %d Improptu_rendezous condition met at node: %d at time: %d with arrival_time: %d\n\n",
				//		 t_id, node, f_time, arrival_time);
				impromptu_count++;				
				//if(!f_stop)
				//	f_stop = true;
				return arrival_time;
			}
		}
	}

	else if( !(is_flat[t_id][node]) && (f_time == t_time) ){
		//printf("\n\n(f) thread: %d Improptu_rendezous condition met at node: %d at time: %d with arrival_time: %d\n\n",
		//			 t_id, node, f_time, arrival_time);
		impromptu_count++;
		//if(!f_stop)
		//	f_stop = true;
		return arrival_time;
	}
	return inf;

}

void Bidirection_search(int src, int dest, int d_time, int LBT, int len, ofstream& out_trace){
	
	int o_id, i_id;
	int i, a_time;
	int min_ford;
	int min_time = time_length;
	int min_trace;
	int last_time = -1;
	int counter=0;
	int threshold = 7;
	f_stop = false;
	impromptu_count = 0;
	omp_set_num_threads(3);
	omp_set_nested(1);

	#pragma omp parallel default(none) private(o_id,obj) firstprivate(window_size,src, dest) shared(min_ford, min_trace, min_time,f_stop, d_time, len, LBT, impromptu_count,theta)
	{
		o_id = omp_get_thread_num();
		if(o_id == 0){
			min_ford = obj.Forward_search(src, dest, d_time, o_id);
			//printf("thread: %d d_time: %d min_ford: %d\n", o_id, d_time, min_ford);
		}
		else if(o_id == 1){
			
			while( (impromptu_count<1) && (f_stop == false) ){
				
				//printf("src:%d dest:%d d_time:%d f_stop: %d impromptus: %d inside loop\n", src, dest, d_time, f_stop, impromptu_count);
			}
			
			if(f_stop == false){
				auto t1 = steady_clock::now();
				auto t2 = steady_clock::now();
				auto duration = duration_cast<milliseconds>(t2-t1).count();
				while(duration < theta){				
					t2 = steady_clock::now();
					duration = duration_cast<milliseconds>(t2-t1).count();
				}
							
			}
			
			f_stop = true;
			
			
			//this_thread::sleep_for(std::chrono::milliseconds(sleep));
			//printf("thread: %d set f_stop\n", o_id);
			
			/*printf("src: %d depature at %d arrival dest: %d at %d  (min_ford: %d,  min_trace: %d)\n\n", 
							src, d_time, dest, min(min_trace, min_ford), min_ford, min_trace);*/
		}
		else{
			omp_set_num_threads(window_size);
			min_trace = inf;
			#pragma omp parallel default(none) private(min_time, i_id,a_time, obj) firstprivate(src, dest, d_time, LBT) shared(min_trace)
			{
				
				i_id = omp_get_thread_num();
				a_time = LBT + d_time + i_id ;
				min_time = obj.Trace_search(src, dest, d_time, a_time, i_id+1);

				//#pragma omp barrier

				#pragma omp critical
				{
					min_trace = min(min_trace, min_time);
				}
				//printf("thread: %d a_time: %d min_time: %d min_trace: %d\n", i_id, a_time, min_time, min_trace);
			}
		}
	} 

	/*printf("src: %d depature at %d arrival dest: %d at %d  (min_ford: %d,  min_trace: %d LBT: %d, len: %d) f_close: %ld t_close: %ld\n\n", 
							src, d_time, dest, min(min_trace, min_ford), min_ford, min_trace, LBT, len, closed[0].size(),closed[1].size());*/

	out_trace << "src: " << to_string(src) << " depature at " << to_string(d_time) << " arrival dest: " << to_string(dest) << " at " << to_string(min(min_trace, min_ford)) << " (min_ford: " << to_string(min_ford) << ", min_trace: " << to_string(min_trace) << " LBT: " << to_string(LBT) << " len: " << to_string(len) << ") f_close: " <<to_string(closed[0].size()) <<" t_close: " << to_string(closed[1].size()) << "\n";
	//printf("min_ford out of region: %d\n", min_ford);
	
	return;
}

int Object::Trace_search(int src, int dest, int d_time, int a_time, int id){
	//printf("(thread: %d) executing Trace_search with arrive time: %d\n\n", id,a_time);
	Queue Q;

	int s,u,w,diff;
	int old_w, prev_w;
	int cont = inf;

	for(auto s : S){
		visited[id][s] = 'U';
		dist[id][s] = -1;
	}

	dist[id][dest] = a_time;
	parent[id][dest] = -1;
	Q.push({0,dest});
	closed[id][dest] = true;
	while(!Q.empty()){

		u = Q.top().second;
		//printf("before while\n");
		/*
		while(!Q.empty() && visited[id][u] == 't'){
			Q.pop();
			u = Q.top().second;
			//printf("while loop in heap case\n");
		}
		//printf("after while\n");
		*/
		diff = Q.top().first;
		w = a_time - diff;
		old_w = w;
		visited[id][u] = 't';

		Q.pop();

		if(f_stop){
			//printf("(thread: %d) either forward search or trace search finished which arrival_time is: %d\n\n", id, a_time);
			return inf;
		}

		if(visited[0][u] == 'f')
			cont = Impromptu_rendezous(0, id, u, dest);
		
		if(cont != inf)
			return cont;

		//usleep(760);
		//printf("arrive %d at %d\n", u, w);
		if( (u == src ) && (w == d_time) ){
			if(!f_stop)
				f_stop = true;
			//printf("depature time at %d is %d from %d with arrive at %d\n\n", src, w, dest, a_time);
			//printf("travel path is: ");
			//Show_r_path(src, id);
			printf("\n");
			parent[id].clear();
			return a_time;
		}
		else if( (u == src) && !(w == d_time) ){
			//printf("indicator d_time: %d  w: %d\n", d_time, w);
			return a_time;
		}

		if(w<d_time){
			//printf("for node: %d exceeds time series:  %d,\n...skipping its updation...\n", u, w);
			return inf;
			//continue;
		}

		for(auto &v : rev_tag_graph[u]){
			bool path = true;
			
			while(v.second[w] == inf){
				//printf("while case w: %d\n", w);
				w -= 1;
				if(w<0){
					path = false;
					break;
				}
				
			}
			
			//in case of no mapping w is choose to largest time less than it;
			
			prev_w = w-1;

			if( (visited[id][v.first] =='U') && path && (dist[id][v.first] < v.second[w]) ){
				dist[id][v.first] = v.second[w];
				parent[id][v.first] = u;
				closed[id][v.first] = true;
				Q.push({a_time-v.second[w], v.first});
				
				// check if time series is flat region or not, if have flat  
				// region, maintain least flat time into another variable.
				
				while(v.second[prev_w] == inf){
					//printf("while case prev_w: %d\n", prev_w);
					prev_w -= 1;
					if(prev_w<=0){
						prev_w = 0;
						break;
					}
				}
				
				if(v.second[prev_w]+1 != v.second[w]){
					is_flat[id][v.first] = true;
					least_flat_time[id][v.first] = v.second[prev_w]+1;
					//printf("flatter node: %c  least time: %d max: %d\n", node[v.first], v.second[prev_w]+1,v.second[w]);
					/*printf("node: %c lower time: %d upper time: %d  prev_w: %d  v.second[prev_w]: %d  is_flat[v.first]: %d\n", 
						node[v.first], least_flat_time[v.first], dist[v.first], prev_w, v.second[prev_w]+1, is_flat[v.first]);*/
				}
			}
			// since in case of non mapping w change from actual value  
			// to less than it we need to restore to actual value.
			w = old_w;
		}
	}
	//Show_path(src, rev_parent);
	//printf("No path exists...\n...searching terminated....\n");
	return inf;
}

int Object::Forward_search(int src, int dest, int time, int id){
	//printf("\n\n(thread: %d) executing Forward_search with depature time: %d\n\n",id, time);
	Queue Q;
	
	int s, u, w;
	int cont = inf;

	for(auto s : S){
		dist[id][s] = inf;
		visited[id][s] = 'U';
	}
	//S.clear();

	dist[id][src] = time;
	parent[id][src] = -1;

	Q.push({dist[id][src], src});
	closed[id][src] = true;
	while(!Q.empty()){
		u = Q.top().second;
		w = Q.top().first;
		visited[id][u] = 'f';

		Q.pop();
		//for(int i=1; i<= limit; i++){
		
		/*for(int i=1; i<=window_size; i++){
			if(visited[i][u] == 't')
			cont =  min(cont, Impromptu_rendezous(id, i, u, dest));
		}

		if(cont != inf)
			return cont;*/

		if(f_stop){
			//printf("(thread: %d) either forward search or trace search finished which depature is: %d\n\n", id, time);
			return inf;
		}
		//usleep(1000);
		//printf("reach to node: %d at %d\n",u,w);

		if(u == dest){
			//printf("Forward_search set f_stop\n");
			if(!f_stop)
				f_stop = true;
			//printf("arrival time at %d is %d from %d depature at %d\n\n", dest, w, src, time);
			//printf("travel path is: ");
			//Show_path(dest, id);
			//printf("\n");
			//Show_dist(src);
			
			return w;
		}

		if(w>=time_length){
			//printf("for node: %d exceeds time series,\n...skipping its updation...\n", u);
			continue;
		}

		for(auto &v : tag_graph[u]){
			if(dist[id][v.first] > v.second[w]){
				dist[id][v.first] = v.second[w];
				parent[id][v.first] = u;
				closed[id][v.first] = true;
				Q.push({dist[id][v.first], v.first});
			}
		}
		
		//Show_dist(src);
	}
	return inf;
}

void Object::Show_path(int dest, int id){
	if(dest != -1){
		printf("%d --> ", dest);
		Show_path(parent[id][dest], id);
	}		
}

void Object::Show_r_path(int dest, int id){
	if(dest != -1){
		Show_r_path(parent[id][dest], id);
		printf("%d --> ", dest);
	}
}

void Readfile(string file){
	string line, x_coord, y_coord, attr;
	string edge_id, src_id, dest_id, dist;
	string time_series, traveltime, travelcost;
	int _edge_id, _src_id, _dest_id;
	/*
		Read Edge.csv file and store { edge_id, src_id, dest_id }
		information into main memory.

		Now Read timeseries.txt file for spatial temporal dataset
		and for respective edge_id, assign travel cost to 
		corresponsing source , destination pair.  
	*/

	//ifstream edgefile("Edges.csv");
	ifstream edgefile(inputfile);
	
	if(!edgefile.is_open()){
		printf("Error: while open file: %s\n",inputfile.c_str());
		_Exit(1);
	}

	getline(edgefile, line, '\n');
	printf("Reading '%s' file with data format: \n", inputfile.c_str());
	cout << line << '\n';

	// A flag for edege is twoWay or not.

	Mark twoway;

	while(edgefile.good()){
		getline(edgefile, x_coord, ',');
		getline(edgefile, y_coord, ',');
		getline(edgefile, src_id, ',');
		getline(edgefile, dest_id, ',');
		getline(edgefile, edge_id, ',');
		getline(edgefile, dist, '\n');

		_edge_id = stoi(edge_id);

		if(edgeinfo.find(_edge_id) != edgeinfo.end() )
			twoway[_edge_id] = true;
		
		edgeinfo[_edge_id] = {stoi(src_id), stoi(dest_id)};

	}
	//Show_edgeinfo();
	edgefile.close();
	
	/*  
		Read timeseries.txt file a time series dataset
		file contain edge_id and time series : cost

		based on edge_id , it get src_id, dest_id 
		and create time aggregrate graph for time series: cost
		from file "timeseries.txt".
	 */

	
	ifstream timeseriesfile(file);


	if(!timeseriesfile.is_open()){
		printf("Error: while open file: %s\n", file.c_str());
		_Exit(1);
	}

	while(timeseriesfile.good()){
		getline(timeseriesfile, line, '\n');

		if(line.length()){
			stringstream ss(line);
			getline(ss, edge_id, '|');
			getline(ss, time_series, '|');

			stringstream tt(time_series);
			_edge_id = stoi(edge_id);
			_src_id = edgeinfo[_edge_id].first;
			_dest_id = edgeinfo[_edge_id].second;
			//cout << _src_id <<' '<< _dest_id << '\n';
			graph[_src_id].insert({_dest_id, 1});


			while(tt >> attr){
				stringstream pp(attr);

				getline(pp, traveltime, ':');
				getline(pp, travelcost, ':');

				gen.push_back({stoi(traveltime), stoi(travelcost)});
				//cout << traveltime << ":" << travelcost << ' ';
			}
			//cout << '\n';

			gen.push_back({stoi(traveltime)+1, 0});
			//cout << edge_id << ' ' << edgeinfo[_edge_id].first << ' ' << edgeinfo[_edge_id].second<< '\n';
			Insert(_edge_id, _src_id, _dest_id);
			
			if(twoway[_edge_id])
				Insert(_edge_id, _dest_id, _src_id);

			gen.clear();
			 
		}
	}

	twoway.clear();
	edgeinfo.clear();
	timeseriesfile.close();		
}

void Insert(int _edge_id, int src_id, int dest_id){
	//printf("call Insert with %d %d %d\n", _edge_id, src_id, dest_id);
	if(tag_graph[src_id].size() == 0){
		tag_graph[src_id] = unordered_map<int, vector<int> > ();
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);

	}
	else if(tag_graph[src_id][dest_id].size() == 0){
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);
	}
	
	for(int i=1; i<gen.size(); i++){
		for(int p = gen[i-1].first; p < gen[i].first; p++){
			tag_graph[src_id][dest_id][p] = p + gen[i-1].second;
		}
	}	
	
	S.insert(src_id);
	S.insert(dest_id);

	return;
}

void Create_ealiest_arrival(){
	for(auto &p : tag_graph){
		for(auto &l : p.second){
			int Min = inf;
			for(int k = time_length-1; k>=0;k--){
				Min = min(Min, l.second[k]);
				if(Min<l.second[k])
					l.second[k] = Min;
			}
		}
	}
}

void Create_reverse_graph(){
    for(auto &p : tag_graph){
        for(auto &l : p.second){
            vector<int> ptr = l.second;
            rev_tag_graph[l.first][p.first] = vector<int>(time_length,inf);
            for(int i=0; i<ptr.size(); i++){
                    if(ptr[i]<time_length)
                        rev_tag_graph[l.first][p.first][ptr[i]] = i;
          	}
        }
    }
}

void Show_tag_graph(){
	for(auto &p : tag_graph){
		//cout << p.first << " | ";
		for(auto &l : p.second){
			cout << node[p.first] << " | " << " --> " <<node[l.first] << " { ";
			vector<int> ptr = l.second;
			for(int i=0; i<ptr.size(); i++)
				if(ptr[i] < inf)
					cout << i <<':' << ptr[i] << ' ';
			cout << "}\n";
		}

		cout << '\n';
	}
	return;
}

void Show_reverse_graph(){
    for(auto &p : rev_tag_graph){
        for(auto &l : p.second){
            cout<< node[p.first] << " | " << " --> " << node[l.first] << " { ";
            vector<int> v = l.second;
            for(int i=0; i<time_length; i++)
                    if(v[i]<inf)
                            cout << i<<':' << v[i] << ' ';
            cout << "}\n";
        }
        cout << '\n';
    }
    return;
}

void Read_bound_file(string file, char type){
	ifstream infile;
	infile.open(file);
	string line;
	int row=0;
	int col;
	int data;
	if(!infile.is_open()){
		cerr << "Error in open file: " << file << endl;
		_Exit(123); 	
	}
	if(type == 'l'){
		while(infile.good()){
			getline(infile, line, '\n');
			stringstream ss(line);
			if(line.length()){
				for(col=0; col < vertex_size; col++){
					ss >> data;			
					Lower_bound[row][col] =  data;		
					//printf("%d  %d ", col, vertex[col]);		
				}
				row += 1;
			}			
			//Halt();	
		}
	}
	else if(type == 'u'){
		while(infile.good()){
			getline(infile, line, '\n');
			stringstream ss(line);
			if(line.length()){
				for(col=0; col < vertex_size; col++){
					ss >> data;			
					//Upper_bound[row][col] =  data;		
					//printf("%d  %d ", col, vertex[col]);		
				}
				row += 1;
			}			
			//Halt();	
		}
	}
	return;
}

void Get_vertex(string file){
	int row = 0;
	ifstream infile;
	infile.open(file);
	string line;
	if(!infile.is_open()){
		cerr << "Error in open file: " << file << endl;
		_Exit(123); 	
	}
	while(infile.good()){
		getline(infile, line, '\n');
		//cout << line << "\n";		
		if(line.length()){
			vertex.insert({stoi(line), row});
			row += 1;
		}
	}
	return;
}

