/*
	This code extracts certain amount of node (50,000)
	using BFS and create a Spatial-Temporal Dataset.

	Base Dataset is a spatial datset set of San Fancisco
	contain Nodes: 439209 Edges: 223000
	available at: https://figshare.com/articles/Urban_Road_Network_Data/2061897

	Average speed in San Francisco : 30 mph ~~  805 meter/minutes
	Dataset format: +proj=utm +zone=43 +datum=WGS84 +units=m +no_defs

*/


#include "dataset.h"

int main()
{
	//string inputfile = "../graphs/SanFrancisco_Edgelist.csv";
	string inputfile = "../graphs/minidata.csv";
	Dataset data(inputfile,25);
	data.Generate_data();
	cout << "\nDone...\n\n";

	return 0;
}