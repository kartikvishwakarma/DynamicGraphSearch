<!-- @author kartik vishwakarma -->

 
#Code for bi-direction search in time-dependent graph.

#-- Required file:

```
	1. vertexfile: path to vertexs information 	of input graph dataset in csv file format as: 
		 {
		 vertex_id1
		 vertex_id2

		 .....
		 vertex_idn
		}.

	2. linkfile: path to edges information of input graph dataset in csv file format as:
	{
		XCoord1,YCoord1,START_NODE1,END_NODE1,EDGE1,LENGTH1
		XCoord2,YCoord2,START_NODE2,END_NODE2,EDGE,LENGTH2

		......
		XCoordn,YCoordn,START_NODEn,END_NODEn,EDGEn,LENGTHn
	}

	3. timeSeriesfile: path to timeseries information of input graph in txt format as:
	{
		edgeID1 | time1_0:cost time1_1:cost ... time1_k:cost

		......
		
		edgeIDn | timen_0:cost timen_1:cost ... timen_k:cost		
	}

	4. LBTfile: path of lower bound of time-dependent graph in txt format of matrix where row-->source node & col-->destination node:

	this file have matrix size of dimension: 
	N x N,  where N : number of vertexs in graph.

	size of this file close to 9GB for N = 50,000

	5. querydir: directory path for all querypoints want to perform bi-directional search

	6. outdir: log file of bi-directional search. 



```


#compile:
	
	open console in code directory and type:
	
	make

#run:
	type in console
	 
	./bidirectional
