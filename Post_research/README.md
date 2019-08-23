/* @author kartik vishwakarma
	2017csm1001@iitrpr.ac.in
*/

## This Readme describes about structure of directory in directory post_research.

#- common: this file is common for all dataset experiments.
	common directory contains 3 files: 
		1. Edges_50000.csv :-  contains 99,999 edges of graph.
		
		2. Nodes_50000.csv :- contains 50,000 Nodes of graph.
		
		3. vertex_order_50000.txt :- this file requied while geeting precalculated Lower and upper bound <src,dest> of algorithms. Since vertex id can be any random value from 1 to any random INT_MAX.
		Maintainng INT_MAX x INT_MAX size matrix not possible in main memory, so this file keeps map for each Node id using 50000 x 50000 matrix.  which is possible on 32 Gb and above memory.


#- datset_1: 35% of 99,999 edges consider as rush edge with 40% max increaes in travel cost.

#- datset_2: 35% of 99,999 edges consider as rush edge with 50% max increaes in travel cost.

#- datset_3: 45% of 99,999 edges consider as rush edge with 40% max increaes in travel cost.

#- datset_4: 45% of 99,999 edges consider as rush edge with 50% max increaes in travel cost.



Each dataset direcory contains following common file:

	1. bidirectional.cpp : implementation of bi-directional Impress based search.

	2. header.h: A cpp header file for required data structure and class

	3. search.h: Implementation of related work, e.g. SP_TAG, Parallel SP_TAG, Distributed heap SP_TAG.

	4. readcases.cpp: A main class file to run "related work" only for given timeseries graph with depature time of {0, 450, 540} respectivly.
	This file creates a Directory for each type of search.



to run "related work" change follwing things line before run:

line:15    type name of directroy want to create

line:111   change type of search you want to run

				e.g.  for SP_TAG:  obj.Forward_search(src, dest, ...)
					  
					  for parallel SP_TAG: obj.parallel_SP_TAG(src, dest, ...) 

					  for distbuted head SP_TAG: dist_SP_TAG(src, dest, ...)

goto any of dataset_{1,2,3,4} directory.
 
##- Compiling code (for related work)
	to complile after changing about mentioned points you just need to compile and run only readcases.cpp file only.

	Compile: g++ -fopenmp -mcmodel=medium readcases.cpp -o readcases -std=c++11

##- Running code
	to run readcases after compile simple type:
	./readcases

running may take time upto 10 minutes to finished....


goto any of datset_{1,2,3,4} directory.

##- To run "bidirectional" search (i.e IMRESS algorithms)
	use bidirectional.cpp file

	change line: 58 for Output dir name

	change line: 61  for theta value 

##- Compiling IMRESS
	type: g++ -fopenmp -mcmodel=medium bidirectional.cpp -o bidirectional -std=c++11

##- Run
	type: ./bidirectional


