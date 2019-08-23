#include "bound.h"

int main()
{
	char type = 'l';
	string outfile = "./lower_bound_50000.txt";
	
	string linkfile = "../dataset/Edges_50000.csv";
	string vertfile = "../dataset/Nodes_50000.csv";
	string timefile = "../dataset/timeseries_50000.txt";


	Bound bd;

	bd.readFile(timefile, linkfile);
	bd.Dijkstra(type);
	bd.showGraph(type);
	bd.writeDist(type,vertfile,outfile);
	
	return 0;
}