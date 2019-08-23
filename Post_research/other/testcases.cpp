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
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <omp.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <regex>

using namespace std;
using namespace std::chrono;

const int time_length = 10080;
const int inf = INT_MAX;
const int window_size = 8;
const int no_of_vertexs = 50000;
//const int no_of_vertexs = 10000;
const int no_src_dest_pair = 10;
const string dir_path = "./Querypoints_10/";
//const string dir_path = "./Datafile_10000/";
const string patt = "(50000_src_dest_)(.*)";
//const string patt = "(10000_src_dest_)(.*)";
const string inputfile = "timeseries_50000.txt";//"../../../Project_work/tdEdgeId.src.sink.time.csv";
//const string inputfile = "../experiments/Urban_Network/testcases/timeseries_10000.txt";

int limit;

int LBT, UBT;

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
typedef std::chrono::duration<int> seconds_type;

TAG_Graph tag_graph, rev_tag_graph;
Edgeinfo edgeinfo;
map<int, map<int, int> > graph;
unordered_set<int> S;
vector<pr> gen;

unordered_map<int, Mark> Present;

char node[] = {'S', 'A', 'C', 'B', 'D', 'E', 'F', 'G'};    // timeseries.txt
//char node[] = {'S', 'A', 'B', 'F', 'D'};				   // timeseries_1.txt
//char node[] = {'S', 'A', 'B', 'C', 'D', 'E', 'F', 'G'};  // timeseries_2.txt
//char node[] = {'A', 'B', 'C', 'D'};

bool f_stop = false;
bool first_impromptu = false;


void Readfile(string file);
void Create_ealiest_arrival(void);
void Insert(int _edge_id, int src_id, int dest_id);
void Show_tag_graph(void);
void traverse_all(string file);
void file_check();
class Object{
	public:
	Queue Q;
	Map dist;
	Map parent;
	Flag visited;


	Object();
	Object(const Object& n);
	int Forward_search(int src, int dest, int d_time, unordered_map<int,int> &items);
	void Show_path(int dest, int id);
	void Show_dist(int src);	
};

Object::Object(){ 
		Q = Queue();
		dist = Map();
		parent = Map();
		visited = Flag(); 
}

Object::Object(const Object& n){
			dist = Map(n.dist);
			parent = Map(n.parent);
			visited = Flag(n.visited);
			Q = Queue(n.Q);
}

Object obj;

const int d_times[] = {0, 540, 600, 1140, 1200};
const int path_lengths[] = {40,60,80,100,120,140,160,180,200,220,240};

int main()
{
	int d_time;
	
	/* mid-night 12:00(free hour), morning 9:00(rush), morning 10:00(non-rush)
		evening 7:00(rush) evening 8:00(non-rush)
	*/

	//inputfile = "../debug/testcases/edge.csv";
	
	auto t1 = steady_clock::now();
	//Readfile("../debug/testcases/timeseries.txt");
	Readfile(inputfile);
	auto t2 = steady_clock::now();
	auto duration = duration_cast<milliseconds>(t2-t1).count();
	cout << "graph creation time: " << duration << '\n';
	cout << "Graph size: " << tag_graph.size() << '\n';
	cout << "Vertex size: " << S.size() << '\n';
	file_check();
	cout << "\n\n";
	string file = "../presearch/1417_nonflat_src_dest_40_510.txt";
	traverse_all(file);	
	return 0;
}

void file_check(){
	
	string filename;
	
	DIR *dp;
	struct dirent *ep;


	dp = opendir(dir_path.c_str());
	/*
	if(dp == NULL){
		perror("Failed to open Directory.");
		_Exit(123);
	}*/

	if(dp == NULL){
		printf("Creating directory: %s\n", dir_path.c_str());
		const int dir_err = mkdir(dir_path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
		    printf("Error creating directory: %s\n", dir_path.c_str());
		    exit(1);
		}
		dp = opendir(dir_path.c_str());
	}
	
	else
		printf("Reading directory: %s\n", dir_path.c_str());

	while(ep = readdir(dp)){
		filename = string(ep->d_name);
		if(regex_match(filename, regex(patt)) )
			remove(ep->d_name);
	}
	closedir(dp);
}

void traverse_all(string file){
	int d_time;
	int src, dest;
	int path_length;

	//for(auto d_time : d_times){
	{
		d_time = 0;
		//printf("d_times\n");
		unordered_map<int, int> items;
		for(auto l : path_lengths)
			items.insert({l,0});
		auto t1 = steady_clock::now();
		
		for(auto s : S){
			auto t2 = steady_clock::now();
			auto duration = duration_cast<seconds>(t2-t1).count();
			if(duration > 60*60)
				break;
			for(auto t : S){
				//printf("t loop\n");
				bool cont = false;
				for(auto &p : items){
					//printf("loop over items\n");
					if(p.second<no_src_dest_pair){
						//printf("cont = true\n");
						cont = true;
						break;
					}
				}
				if(cont){
					//printf("Forward_search\n");
					obj.Forward_search(s, t, d_time, items);
				}
			}
			duration_cast<milliseconds>(t2-t1).count();
		}
	}
}


int Object::Forward_search(int src, int dest, int time, unordered_map<int, int> &items){
	//printf("\n src: %d dest: %d time: %d \n\n", src, dest,time);
	Queue Q;
	
	int s, u, w;
	int cont = inf;

	for(auto s : S){
		dist[s] = inf;
		visited[s] = 'U';
	}
	//S.clear();

	dist[src] = time;
	parent[src] = -1;

	Q.push({dist[src], src});

	while(!Q.empty()){
		u = Q.top().second;
		w = Q.top().first;
		visited[u] = 'f';

		Q.pop();

		if(u == dest){
			for(auto len : path_lengths){
				if(len == (w -time) && (items[len] < no_src_dest_pair) && !Present[len][src] && !Present[len][dest]){
					string file = dir_path + to_string(no_of_vertexs) +  + "_src_dest_" + to_string(time) + "_" + to_string(len) + ".txt";
					ofstream outfile;
					outfile.open(file, ios_base::app);

					if(!outfile.is_open()){
						printf("ERROR!! Open file: %s\n", file.c_str());
						_Exit(235);
					}
					outfile << src << ' ' << dest  << ' ' << len << '\n';
					outfile.close();
					items[len] += 1;
					Present[len][src] = true;
					Present[len][dest] = true;
				}
			}

			//printf("\n");
			//Show_dist(src);
			
			return w;
		}

		if(w>=time_length){
			//printf("for node: %d exceeds time series,\n...skipping its updation...\n", u);
			continue;
		}

		for(auto &v : tag_graph[u]){
			if(dist[v.first] > v.second[w]){
				dist[v.first] = v.second[w];
				parent[v.first] = u;
				Q.push({dist[v.first], v.first});
			}
		}
		
		//Show_dist(src);
	}
	return inf;
}

void Object::Show_path(int dest, int id){
	if(dest != -1){
		printf("%d --> ", dest);
		Show_path(parent[dest], id);
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
	//string str = "Edges_" + to_string(no_of_vertexs) + ".csv";
	string str = "../experiments/Urban_Network/testcases/edges_10000.csv";
	ifstream edgefile(str);

	if(!edgefile.is_open()){
		cout << "Error: while open file " << str << "\n";
		_Exit(1);
	}

	getline(edgefile, line, '\n');
	printf("Reading 'Edegs.csv' file with data format: \n");
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
		printf("Error: while open timeseries.txt file \n");
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
			}

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

	for(int i = 1; i < gen.size(); i++){
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
