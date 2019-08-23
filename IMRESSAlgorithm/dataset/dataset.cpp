#include "dataset.h"

Dataset::Dataset(string file, int s, int nodes,
	double p, double amt)
{
	filename = file;
	src = s;
	no_of_nodes = nodes;
	perct = p;
	amount = amt; 
}

/*Dataset::~Dataset()
{
	
}*/

void Dataset::Generate_data()
{
	cout<<"Extracting graph information from file: " << filename << endl;
	Readfile();

	
	BFS();
	cout << "Done graph information extraction." << endl;
	
	cout << "Creating time-dependent graph dataset." << endl;
	CreateEdgeList();
	
	int size = ceil(double(edges.size())*perct);

	srand(time(0));

	
	while(ID.size() < size)
	{
		auto k = edges.begin();
		int p = rand() % size;
		advance(k,p);

		if(ID.find(*k) == ID.end())
			ID.insert(*k);
	}
	TimeSeriesDataset();
	edges.clear();
	ID.clear();

	cout << "Done with time-dependent graph creation." << endl;

	return;
}


void Dataset::TimeSeriesDataset()
{
	int day = 0;
	int travel_cost;
	int rush_travel_cost;
	double rush_time;
	int s_id, d_id, e_id;
	int period;

	ofstream outfile;
	string file = "timeseries_" + to_string(no_of_nodes) + ".txt";

	outfile.open(file);

	for(auto p : ex_graph)
	{
		e_id = p[0];
		s_id = p[1];
		d_id = p[2];
		travel_cost = p[3];
				
		if(travel_cost == 0)
			travel_cost += 1;

		outfile << e_id << " | ";

		if(ID.find(e_id) != ID.end())
		{
			for(int day=0; day<time_length/1440; day++)
			{				
				period = 1440 * day;
				
				outfile << ' ' << to_string(period) << ':' << to_string(travel_cost);
				
				int prev_val = -1;
				for(int time = 480; time < 660; time++)
				{	
					// generate sine wave with alpha = 1.0
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
				for(int time = 1080; time < 1260; time++)
				{	
					// generate mini sin wave with alpha = 0.75
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
		else
		{
			for(int day=0; day<time_length/1440; day++)
			{
				//travel_cost = ceil(p[3]/speed);
				outfile << ' ' << to_string(1440 * day) << ':' << to_string(travel_cost);
			}
		}

		outfile<< ' ' << to_string(time_length) << ':' << to_string(travel_cost) << " |\n";

	}
	outfile.close();
}



void Dataset::CreateEdgeList()
{
	ofstream outfile;
	string file = "Edges_" + to_string(no_of_nodes) + ".csv";

	outfile.open(file);
	
	if(!outfile.is_open())
	{
		cout << "ERROR: in open file, " << file << '\n';
		_Exit(1);	
	}

	cout << "\n\nCreating Dataset in format:\n"; 
	cout <<"XCoord,YCoord,START_NODE,END_NODE,EDGE,LENGTH\n\n";
	
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

void Dataset::Readfile()
{
	string line, X_coord, Y_coord;
	string edges_id, src_id, dest_id, dist;
	int _edges_id, _src_id, _dest_id, _dist;

	ifstream infile(filename);

	if(!infile.is_open())
	{
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

		/**************************
		*distance given in double.*
		**************************/

		_edges_id = stoi(edges_id);
		_src_id = stoi(src_id);
		_dest_id = stoi(dest_id);
		_dist = stoi(dist);

		if(twoway[_edges_id])
			twoway[_edges_id] = false;
		else
			twoway[_edges_id] = true;

		//cout << _src_id << ' ' << _dest_id << ' ' << _edges_id << endl;
		graph[_src_id].push_back({_dest_id, _edges_id});
		datainfo[_edges_id] = vector<int> {_edges_id, _src_id, _dest_id, _dist/speed};
		coordinates[_edges_id] = {X_coord, Y_coord};
		S.insert(_src_id);
		S.insert(_dest_id);
	}

	int links = datainfo.size();
	int nodes = S.size();
	cout << "\nGraph details: " << endl; 
	cout << "#Nodes: "<< nodes << endl;
	cout << "#Edges: "<< links << endl;
	cout << "Avg. Degree: " << double(2*links)/nodes << endl; 
}

void Dataset::BFS()
{
	queue<int> Q;
	ofstream outfile;

	for(auto s : S)
		visited[s] = false;

	Q.push(src);
	visited[src] = true;
	int line=0;
	vertex.insert(src);

	while(!Q.empty() && vertex.size() < no_of_nodes){
		//cout << "inside BFS loop\n";
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
	
	// writing extracted sub graph into file.
	string file = "Nodes_" + to_string(no_of_nodes) + ".csv";
	outfile.open(file);
	for(auto p : vertex)
		outfile << p << '\n';
	outfile.close();


	int links = edges.size();
	int nodes = vertex.size();

	cout << "\nDynamic Graph details: " << endl; 
	cout << "#Nodes: "<< nodes << endl;
	cout << "#Edges: "<< links << endl;
	cout << "Avg. Degree: " << double(2*links)/nodes << endl;
	vertex.clear();
	graph.clear();
	visited.clear();
}

void Dataset::show_edges()
{
	for(auto p : edges)
		cout << p << endl;
}

double Dataset::sine_fun(double alpha, double x)
{
	return alpha*(std::sin(double(2*pi*f*x)/double(fs)));
}
