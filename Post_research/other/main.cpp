/*
	This code extracts certain amount of node (50,000)
	using BFS and create a Spatial-Temporal Dataset.

	Base Dataset is a spatial datset set of San Fancisco
	contain Nodes: 174955 Edges: 223000
	available at: https://www.cs.utah.edu/~lifeifei/SpatialDataset.htm

	Average speed in San Francisco : 30 mph ~~  805 meter/minutes
	Dataset format: +proj=utm +zone=43 +datum=WGS84 +units=m +no_defs

*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;

const int minval = 1;
const int time_length = 10080;
const int speed = 805; // mph

const double pi = 3.14159;
const double f = 0.5;
const int fs = 180;

typedef pair<int, int> pr;
typedef map<int, int> Map;
typedef vector<int> vect;
typedef map<int, vect> Data;
typedef map<int , vector<pr> > Graph;
typedef vector<vect> Ex_Graph;
typedef unordered_set<int> Set;

map<int, bool> visited;
map<int, bool> twoway;
map<int, pair<string, string> > coordinates;
Set S, ID, vertex, edges; 
Map travel_cost;
Data datainfo;
Graph graph;
Ex_Graph ex_graph;
Map degree;


void Show_edges();
void Readfile(string filename);
void BFS(int src, int no_of_nodes);
void CreateEdgeList(int no_of_nodes);
void TimeSeriesDataset(double amount, int no_of_nodes);
void modified_TimeSeriesDataset(double amount);
void Generate_data(int no_of_nodes, double perct, double amount);
double sine_fun(double alpha, double x);

int main(int argc, char const *argv[])
{
	// total number of nodes (approx.)
	int no_of_nodes = 50000;
	// percentage of edges having congestion 
	double perct = 0.35;
	// increase in travel cost during congention 
	double amount = 0.40;//10;

	// function to create Spatial-temporal Dataset
	Generate_data(no_of_nodes, perct, amount);

	return 0;
}

void Generate_data(int no_of_nodes, double perct, double amount){
	//string filename = "../../Urban_Road_Network/SanFrancisco/SanFrancisco_Edgelist.csv";
	string filename = "../experiments/Urban_Network/others/SanFrancisco_Edgelist.csv";	

	int src = 7320;
	// Read given spatial data edge information.
	printf("Readfile(filename)\n");
	Readfile(filename);


	//Extracting connected nodes and edges applying BFS
	/*for(auto p : S)
		BFS(p, no_of_nodes);*/
	printf("BFS(src, no_of_nodes)\n");
	BFS(src, no_of_nodes);
	
	int size = ceil(double(edges.size())*perct);
	//printf("%d\n", size);
	//Show_edges();
	printf("CreateEdgeList()\n");
	CreateEdgeList(no_of_nodes);
	
	srand(time(0));

	while(ID.size() < size){
		auto k = edges.begin();
		//printf(".");
		int p = rand() % size;
		advance(k, p);
		//cout << *k << ' ' << ID.size() << '\n';
		if(ID.find(*k) == ID.end())
			ID.insert(*k);
	}
	edges.clear();
	printf("\n");

	printf("TimeSeriesDataset(amount)\n");
	//TimeSeriesDataset(amount);
	TimeSeriesDataset(amount, no_of_nodes);
	ID.clear();
	
}

double sine_fun(double alpha, double x){
	return alpha*sin(double(2*pi*f*x)/(double)fs);
}

void Show_edges(){
	for(auto p : edges)
		cout << p << '\n'; 
}

void TimeSeriesDataset(double amount, int no_of_nodes){
	int day = 0;
	int travel_cost;
	int rush_travel_cost;
	double rush_time;
	int s_id, d_id, e_id;
	int period;

	ofstream outfile;
	string file = "timeseries_" + to_string(no_of_nodes) + ".txt";

	outfile.open(file);

	for(auto p : ex_graph){
		e_id = p[0];
		s_id = p[1];
		d_id = p[2];
		travel_cost = p[3];
				
		if(travel_cost == 0)
			travel_cost += 1;

		outfile << e_id << " | ";

		if(ID.find(e_id) != ID.end()){
			for(int day=0; day<time_length/1440; day++){
				

				period = 1440 * day;
				
				outfile << ' ' << to_string(period) << ':' << to_string(travel_cost);
				//printf("edge_id: %d  %d %lf\n", e_id, period, double(travel_cost));

				int prev_val = -1;
				for(int time = 480; time < 660; time++){
					// generate sine wave with alpha = 1.0
					//printf("for x: %d  sin(x): %lf travel_cost: %d\n", time-480, double(travel_cost)*(sine_fun(1.0, time-480)*amount + 1.0), travel_cost);
					rush_time = (double(travel_cost)*(sine_fun(1.0, time-480)*amount+1.0));
					//printf("rush_time: %lf  rush_travel_time: %d\n", rush_time, int(rush_time));					
					rush_travel_cost = ceil(rush_time);
					if(rush_travel_cost != prev_val){
						outfile << ' ' << to_string(period + time) << ':' << to_string(rush_travel_cost);
						//printf("edge_id: %d  %d %lf\n", e_id, period+time, rush_time);
						prev_val = rush_travel_cost;
					}
				}
				//printf("edge_id: %d  %d %lf\n", e_id, period+660, double(travel_cost));
				outfile	<< ' ' << to_string(period + 660) << ':' << to_string(travel_cost);
				prev_val = -1;
				for(int time = 1080; time < 1260; time++){
					// generate mini sin wave with alpha = 0.75
					//printf("for x: %d  sin(x): %lf travel_cost: %d\n", time-1080, double(travel_cost)*(sine_fun(0.75, time-1080)*amount+1.0), travel_cost);
					rush_time = double(travel_cost)*(sine_fun(0.75, time-1080)*amount+1.0);
					//printf("rush_time: %lf  rush_travel_time: %d\n", rush_time, int(rush_time));
					rush_travel_cost = ceil(rush_time);
					if(rush_travel_cost != prev_val){
						outfile << ' ' << to_string(period + time) << ':' << to_string(rush_travel_cost);
						//printf("edge_id: %d  %d %lf\n", e_id, period+time, rush_time);
						prev_val = rush_travel_cost;
					}
				}

				outfile << ' ' << to_string(period + 1260) << ':' << to_string(travel_cost);
				//printf("edge_id: %d  %d %lf\n", e_id, period+1260, double(travel_cost));
			}
			//printf("edge_id: %d  %d %lf\n", e_id, time_length, double(travel_cost));
		}
		else{
			for(int day=0; day<time_length/1440; day++){
				//travel_cost = ceil(p[3]/speed);
				outfile << ' ' << to_string(1440 * day) << ':' << to_string(travel_cost);
			}
		}

		outfile<< ' ' << to_string(time_length) << ':' << to_string(travel_cost) << " |\n";

	}
	outfile.close();
}


void modified_TimeSeriesDataset(double amount){
	int day = 0;
	int travel_cost;
	int rush_travel_cost;
	double rush_time;
	int s_id, d_id, e_id;
	int period;

	ofstream outfile;

	outfile.open("timeseries.txt");

	for(auto p : ex_graph){
		e_id = p[0];
		s_id = p[1];
		d_id = p[2];
		travel_cost = p[3];
				
		if(travel_cost == 0)
			travel_cost += 1;

		outfile << e_id << " | ";

		if(ID.find(e_id) != ID.end()){
			for(int day=0; day<time_length/1440; day++){
				//rush_travel_cost = ceil((int)(ceil(p[3]*(1.0+amount)))/speed);
				//travel_cost = ceil(p[3]/speed);
				

				period = 1440 * day;
				
				outfile << ' ' << to_string(period) << ':' << to_string(travel_cost);
				//printf("edge_id: %d  %d %lf\n", e_id, period, double(travel_cost));

				double prev_val = -1.0;
				for(int time = 480; time < 660; time++){
					// generate sine wave with alpha = 1.0
					//printf("for x: %d  sin(x): %lf\n", time-480, double(travel_cost)*(sine_fun(1.0, time-480)*amount+1.0) );
					rush_time = (double(travel_cost)*(sine_fun(1.0, time-480)*amount+1.0));
					printf("%d,%lf\n", time-480, rush_time);
					rush_travel_cost = rush_time;
					if(rush_travel_cost != prev_val){
						outfile << ' ' << to_string(period + time) << ':' << to_string(rush_travel_cost);
						//printf("edge_id: %d  %d %lf\n", e_id, period+time, rush_time);
						prev_val = rush_time;
					}
				}
				//printf("edge_id: %d  %d %lf\n", e_id, period+660, double(travel_cost));
				outfile	<< ' ' << to_string(period + 660) << ':' << to_string(travel_cost);
				prev_val = -1;
				for(int time = 1080; time < 1260; time++){
					// generate mini sin wave with alpha = 0.75
					rush_time = ceil(double(travel_cost)*(sine_fun(0.75, time-1080)*amount+1.0));
					rush_travel_cost = rush_time;
					if(rush_travel_cost != prev_val){
						outfile << ' ' << to_string(period + time) << ':' << to_string(rush_travel_cost);
						//printf("edge_id: %d  %d %lf\n", e_id, period+time, rush_time);
						prev_val = rush_time;
					}
				}

				outfile << ' ' << to_string(period + 1260) << ':' << to_string(travel_cost);
				//printf("edge_id: %d  %d %lf\n", e_id, period+1260, double(travel_cost));
			}
			//printf("edge_id: %d  %d %lf\n", e_id, time_length, double(travel_cost));
		}
		else{
			for(int day=0; day<time_length/1440; day++){
				//travel_cost = ceil(p[3]/speed);
				outfile << ' ' << to_string(1440 * day) << ':' << to_string(travel_cost);
			}
		}

		outfile<< ' ' << to_string(time_length) << ':' << to_string(travel_cost) << " |\n";

	}
	outfile.close();
}



void CreateEdgeList(int no_of_nodes){
	ofstream outfile;
	string file = "Edges_" + to_string(no_of_nodes) + ".csv";

	outfile.open(file);
	outfile << "XCoord,YCoord,START_NODE,END_NODE,EDGE,LENGTH\n";
	for(auto p : ex_graph){
		outfile << coordinates[p[0]].first << ',' << coordinates[p[0]].second << ','
				<< to_string(p[1]) << ',' << to_string(p[2]) << ','
			 	<< to_string(p[0]) << ',' << to_string(p[3]) << '\n';
	}
	outfile.close();
	//cout << "vertex size: " << tmp.size() << '\n';
	coordinates.clear();
	
}

void BFS(int src, int no_of_nodes){
	queue<int> Q;
	ofstream outfile;

	for(auto s : S)
		visited[s] = false;

	Q.push(src);
	visited[src] = true;
	int line=0;
	vertex.insert(src);

	while(!Q.empty() && vertex.size() < no_of_nodes){
		int u = Q.front();
		Q.pop();
		
		for(auto v : graph[u]){
			if(!visited[v.first]){
				visited[v.first] = true;
				Q.push(v.first);
				vertex.insert(v.first);
				edges.insert(v.second);
				ex_graph.push_back(datainfo[v.second]);
				
				// since for bidirection edges have same id
				// reinsert their details swaping { src, dest } 

				if(twoway[v.second] == false){
					int e_id = datainfo[v.second][0];
					int d_id = datainfo[v.second][1];
					int s_id = datainfo[v.second][2];
					int t_id = datainfo[v.second][3];
					vector <int> vec = {e_id, s_id, d_id, t_id};
					ex_graph.push_back(vec);
				}
			}
		}
	}
	//cout << items << '\n';
	cout<<src <<":  " << vertex.size()<< ' ' << edges.size() << '\n';
	
	string file = "Nodes_" + to_string(no_of_nodes) + ".csv";

	outfile.open(file);
	

	for(auto p : vertex)
		outfile << p << '\n';
	outfile.close();

	vertex.clear();
	graph.clear();
	visited.clear();
	//identi.clear();

}

void Readfile(string filename){
	string line, X_coord, Y_coord;
	string edges_id, src_id, dest_id, dist;
	int _edges_id, _src_id, _dest_id, _dist;
	//double _dist;
	
	ifstream infile(filename);

	if(!infile.is_open()){
		cout << "ERROR: in open file, " << filename << '\n';
		_Exit(1);	
	}

	getline(infile, line, '\n');
	while(infile.good()){
		getline(infile, X_coord, ',');
		getline(infile, Y_coord, ',');
		getline(infile, src_id, ',');
		getline(infile, dest_id, ',');
		getline(infile, edges_id, ',');
		getline(infile, dist, '\n');

		//stringstream ss(line);

		//ss >> edges_id >> src_id >> dest_id >> dist;
		
		/**************************
		*distance given in double.*
		**************************/

		_edges_id = stoi(edges_id);
		_src_id = stoi(src_id);
		_dest_id = stoi(dest_id);
		_dist = stoi(dist);
		//_dist = stof(dist);

		/*printf("src: %d  dest: %d edges_id: %d  dist: %d\n",
			_src_id, _dest_id, _edges_id, _dist);*/

		if(twoway[_edges_id])
			twoway[_edges_id] = false;
		else
			twoway[_edges_id] = true;

		graph[_src_id].push_back({_dest_id, _edges_id});
		datainfo[_edges_id] = vector<int> {_edges_id, _src_id, _dest_id, _dist/speed};
		coordinates[_edges_id] = {X_coord, Y_coord};
		S.insert(_src_id);
		S.insert(_dest_id);


		/*degree[_src_id] += 1;
		degree[_dest_id] += 1;*/
	}
	printf("Graph size: %ld\n", S.size());
	/*for(auto &p : degree)
		cout << p.first << ' ' << p.second << '\n';*/
}
