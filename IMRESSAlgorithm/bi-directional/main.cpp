/* g++ -fopenmp  -mcmodel=medium bidirectional.cpp -o bidirectional -std=c++11 -g 
   for 50,000 graph preprocessing time approx. 15 minutes.
   preprocessing include:
			-- loading lower and upper bound 
			-- graph creation
			-- earliest arrival graph
			-- reverse graph creation
*/


#include "Bidirectional.h"

int main()
{
	string vetexfile = "../dataset/Nodes_50000.csv";
	string linkfile = "../dataset/Edges_50000.csv";
	string timeSeriesfile = "../dataset/timeseries_50000.txt";
	string LBTfile = "../dataset/lower_bound_5000.txt";
	
	string dir = "../<file_path>";
	string outdir = "../<file_path>";
	string querydir = "../<file_path>";

	cout << "main function()\n";
	
	Bidirectional *bidir = new Object(3);
	bidir->getVertex(vetexfile);
	//bidir->readBoundFile(LBTfile, 'l');
	bidir->readTimeSeries(timeSeriesfile,linkfile);
	bidir->createEarliestArrival();
	bidir->createReverseGraph();

	//bidir->readCase(dir,querydir,outdir);
	
	ofstream out;
	out.open(dir);
	bidir->bidirectionalSearch(295676,268748,0,2,200,out);
	
	
	bidir->showTAGraph();
	return 0;
}