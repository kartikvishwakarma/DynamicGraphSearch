#ifndef BOUND_H
#define BOUND_H

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

#include <cstdio>
#include <cstdlib>
#include <climits>

#define inf INT_MAX

using namespace std;

class Bound
{
private:
	typedef pair<int,int> pr;
	vector<pr> gen;
	unordered_set<int> vertex;
	unordered_map<int,bool> visited;
	unordered_map<int, pr> edgeinfo;
	unordered_map<int, vector<pr> > minGraph;
	unordered_map<int, vector<pr> > maxGraph;
	unordered_map<int, unordered_map<int,int> > dist_matrix;
	priority_queue<pr, vector<pr>, greater<pr> > Q;

public:
	Bound();

	void Dijkstra(char type = 'l');
	void readFile(string timefile, string linkfile);
	void Insert(int _edge_id, int src_id, int dest_id);
	void showGraph(char type);
	void writeDist(char type, string vertfile, string outfile);
	
	vector<pr>& getlink(bool type, int u)
	{
		if(type == 'l')
			return minGraph[u];
		return maxGraph[u];
	}

};


#endif