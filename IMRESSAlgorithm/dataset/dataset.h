#ifndef DATASET_H
#define DATASET_H

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

class Dataset
{
private:

	string filename;
	int src;
	int no_of_nodes;
	double amount;
	double perct;

	double x;
	double alpha;
	
	unordered_set<int> S;
	unordered_set<int> ID;
	unordered_set<int> edges;
	unordered_set<int> vertex;

	map<int,bool> twoway;
	map<int,bool> visited;
	map<int, vector<int> > datainfo;
	map<int, vector<pair<int,int> > > graph;
	map<int, pair<string,string> >coordinates; 

	vector<vector<int> > ex_graph;

	
public:

	Dataset(string file, int s=7320, int nodes=50000,
		double p=0.35, double amt=0.40);
	void show_edges();
	void Readfile();
	void BFS();
	void CreateEdgeList();
	void TimeSeriesDataset();
	void modified_TimeSeriesDataset(double amount);
	void Generate_data();
	double sine_fun(double alpha, double x);

	//~Dataset();
	
};

#endif