#ifndef BI_DIRECTIONAL_H
#define BI_DIRECTIONAL_H

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

#define inf INT_MAX

using namespace std;
using namespace std::chrono;

class Bidirectional
{
private:
	int size;
	int theta;
	int vertexSize;
	int **LowerBound;
	int **UpperBound;
	typedef unordered_map<int,vector<int> >vect;
	unordered_set<int> S;
	vector<pair<int,int> > gen;
	unordered_map<int, pair<int,int> > edgeinfo;
	unordered_map<int,int> vertex;
	unordered_map<int, vect> tag_graph;
	unordered_map<int, vect> rev_tag_graph;

public:
	static int time_length;
	static bool f_stop;
	static int impromptu_count;

	Bidirectional(int size=10, int th=20);
	~Bidirectional();

	void showTAGraph();
	void showRevTAGrap();
	void getVertex(string file);
	void readTimeSeries(string file, string infile);
	void readCase(string dir, string qdir, string opdir);
	void readBoundFile(string file, char type);
	void createEarliestArrival();
	void createReverseGraph();
	void Insert(int edge, int src, int dest);
	void bidirectionalSearch(int src, int dest, int d_time, int LBT, int len, ofstream& out_trace);

	virtual int closeSize(int id) = 0;
	virtual int forwardSearch(int src, int dest, int d_time, int id) = 0;
	virtual int traceSearch(int src, int dest, int d_time, int a_time, int id) = 0;

	vect& getGraph(int u)
	{
		return tag_graph[u];
	}

	vect& getReverseGraph(int u)
	{
		return rev_tag_graph[u];
	}

	unordered_set<int>& getSet(){
		return S;
	}

};


class Object : public Bidirectional
{
private:

	typedef pair<int,int> pr;
	typedef priority_queue<pr, vector<pr>, greater<pr> > Queue;
	
	Queue *Q;
	unordered_map<int,int> *dist;
	unordered_map<int,int> *parent;
	unordered_map<int,int> *least_flat_time;
	unordered_map<int,char> *visited;
	unordered_map<int,char> *is_flat;
	unordered_map<int,bool> *closed;
	

public:
	Object(int size=10, int th=20);
	Object(const Object& obj);
	Object& operator=(const Object &obj);
	int forwardSearch(int src, int dest, int d_time, int id);
	int traceSearch(int src, int dest, int d_time, int a_time, int id);
	int impromptuRendezous(int fid, int tid, int node, int dest);
	int closeSize(int id);

	void showPath(int dest, int id);
	void showRevPath(int dest, int id);

};

#endif