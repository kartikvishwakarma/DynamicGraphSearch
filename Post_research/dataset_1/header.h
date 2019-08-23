#ifndef HEADER_H
#define HEADER_H

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

using namespace std;
using namespace std::chrono;

const int time_length = 10080;
const int inf = INT_MAX;
const int window_size = 8;
int limit;

int LBT, UBT;

typedef pair<int, int> pr;
typedef vector<pr> Link;
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
vect mapper;
Mark visited;

char node[] = {'S', 'A', 'C', 'B', 'D', 'E', 'F', 'G'};    // timeseries.txt
string inputfile;
bool f_stop = false;
bool first_impromptu = false;


void Readfile(string file);
void Create_ealiest_arrival(void);
void Insert(int edge_id, int src, int dest);
void Show_tag_graph(void);
void traverse_all(string file);
void dist_SP_TAG(int src, int dest, int time, int threads, int len);

class Object{
	public:
	Queue Q;
	Map dist;
	Map parent;
	Flag visited;


	Object();
	Object(const Object& n);
	int Forward_search(int src, int dest, int d_time, int id, ofstream& out_trace);
	int parallel_SP_TAG(int src, int dest, int time,int len);
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


class MinHeap{
	Link heap;
	int n;
public:
	MinHeap();
	void push(pr data);
	void pop();
	pr top();
	bool empty();
	int size();
	int parent(int);
	void swap(pr&, pr&);
	void decreaseKey(int, int);
	void remove(int);
	void MinHeapify(int);
	void show();
};

MinHeap::MinHeap(){
	Link heap = Link();
}

void MinHeap::push(pr data){
	heap.push_back(data);
	int i = heap.size()-1;

	while(i != 0 && heap[parent(i)].second > heap[i].second){
		swap(heap[i], heap[parent(i)]);
		i = parent(i);
	}
}

int MinHeap::parent(int i){
	return (i-1)/2;
}

void MinHeap::swap(pr& a, pr& b){
	pr tmp = a;
	a = b;
	b = tmp;
}

pr MinHeap::top(){
	if(heap.size())
		return heap[0];
	return {inf,inf};
}

void MinHeap::decreaseKey(int i, int data){
	heap[i].second = data;
	while(i != 0 && heap[parent(i)].second > heap[i].second){
		swap(heap[parent(i)], heap[i]);
		i = parent(i);
	}
}

void MinHeap::remove(int i){
	decreaseKey(i, INT_MIN);
	pop();
}

void MinHeap::pop(){
	int n = heap.size();
	heap[0] = heap[n-1];
	heap.erase(heap.end());
	MinHeapify(0);
}

void MinHeap::MinHeapify(int i){
	int l = 2*i + 1;
	int r = 2*i + 2;
	int min = i;
	int n = heap.size();

	if(l < n && heap[l].second < heap[i].second)
		min = l;
	if(r < n && heap[r].second < heap[min].second)
		min = r;

	if(min != i){
		swap(heap[min], heap[i]);
		MinHeapify(min);
	}
}

bool MinHeap::empty(){
	if(heap.size()==0)
		return true;

	return false;
}

void MinHeap::show(){
	for(auto p : heap)
		cout << '(' << p.first << ',' << p.second <<") ";
	cout << '\n';
}

#endif
