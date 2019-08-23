#include "bound.h"

Bound::Bound()
{

}

void Bound::Dijkstra(char type)
{
	int u,v,w;
 
	for(int src : vertex)
	{
		cout << "compting all pair shortest path w.r.t. node: " << src << endl;
		for(int v : vertex)
			dist_matrix[src][v] = inf;
		
		Q.push({0,src});

		while(!Q.empty())
		{
			u = Q.top().second;
			w = Q.top().first;
			Q.pop();
			
			for(auto v : getlink(type,u))
			{
				if(dist_matrix[src][v.first] > w + v.second)
				{
					dist_matrix[src][v.first] = w + v.second;
					Q.push({w+v.second, v.first});
				}
			}
		}	
	}
}

void Bound::readFile(string timefile, string linkfile)
{
	cout << "creating graph using file: " << linkfile << endl;

	string line, x_coord, y_coord, attr;
	string edge_id, src_id, dest_id, dist;
	string time_series, traveltime, travelcost;
	int _edge_id, _src_id, _dest_id;

	ifstream edgefile(linkfile);

	if(!edgefile.is_open())
	{
		cout << "Error: while open file: " << linkfile << endl;
		_Exit(1);
	}

	getline(edgefile, line, '\n');

	unordered_map<int,bool> twoway;

	while(edgefile.good())
	{
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

	edgefile.close();

	ifstream timeseriesfile(timefile);

	if(!timeseriesfile.is_open())
	{
		cout << "Error while open file: " << timefile << endl;
		_Exit(1);
	}

	while(timeseriesfile.good())
	{
		getline(timeseriesfile, line, '\n');

		if(line.length())
		{
			stringstream ss(line);
			getline(ss, edge_id, '|');
			getline(ss, time_series, '|');

			stringstream tt(time_series);
			_edge_id = stoi(edge_id);
			_src_id = edgeinfo[_edge_id].first;
			_dest_id = edgeinfo[_edge_id].second;
			//cout << _src_id <<' '<< _dest_id << '\n';

			while(tt >> attr)
			{
				stringstream pp(attr);

				getline(pp, traveltime, ':');
				getline(pp, travelcost, ':');
				gen.push_back({stoi(traveltime), stoi(travelcost)});
			}
			gen.push_back({stoi(traveltime)+1, 0});
			//cout << edge_id << ' ' << edgeinfo[_edge_id].first << ' ' << edgeinfo[_edge_id].second<< '\n';
			Insert(_edge_id, _src_id, _dest_id);
			
			if(twoway[_edge_id])
				Insert(_edge_id, _dest_id, _src_id);

			gen.clear();
			 
		}
	}
}

void Bound::Insert(int _edge_id, int src_id, int dest_id)
{
	int min_time = INT_MAX;
	int max_time = INT_MIN;

	for(int i=1; i<gen.size(); i++)
	{
		for(int p = gen[i-1].first; p < gen[i].first; p++){
			min_time = min(min_time, gen[i-1].second);
			max_time = max(max_time, gen[i-1].second);
		}
	}
	minGraph[src_id].push_back({dest_id, min_time});
	maxGraph[src_id].push_back({dest_id, max_time});

	vertex.insert(src_id);
	vertex.insert(dest_id);
}

void Bound::showGraph(char type)
{	

	for(auto p : vertex){
		cout << p << " | ";
		for(auto l : getlink(type,p))
			cout << " --> " << l.first << ':' << l.second ;
		cout << '\n';
	}
}

void Bound::writeDist(char type, string vertfile, string outfile)
{
	ofstream file;
	file.open(vertfile);

	if(!file.is_open())
	{
		cout << "Error: while open file: " << vertfile << endl;
		_Exit(1);
	}

	for(auto &p : dist_matrix)
		file << to_string(p.first) << ' ';
	file << '\n';
	file.close();

	ofstream datafile;
	datafile.open(outfile);
	
	if(!datafile.is_open())
	{
		cout << "Error: while open file: " << outfile << endl;
		_Exit(1);
	}

	for(auto &d : dist_matrix)
	{
		for(auto &p : d.second)
			datafile << to_string(p.second) << ' ';
		datafile << '\n';
	}
	datafile.close();

}