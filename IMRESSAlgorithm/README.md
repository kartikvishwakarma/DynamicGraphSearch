<!-- 
@author kartik vishwakarma
2017csm1001@iitrpr.ac.in
 -->


# Time-dependent Graph search Optimization
This work optimize graph search algorithms shortest path (here) for a time varying environment using Idea of bidirctional search.


this repository has work as below.

```
		graphs/
		  |
	  	  |
	  	  V
	   dataset/
		  |
	  	  |
	  	  V
  	    bound/
  	      |
	  	  |
	  	  V
  	  bi-directional/

```
```
	graphs/ : contain various cities road networks graph data. 
	e.g.  San Fancisco,  California

	for more dataset:
	https://figshare.com/articles/Urban_Road_Network_Data/2061897

	dataset/ : extract graph information in required format for build graph data structure.

	bound/ : precompute and store lower and upper bound of graphs

	bi-directional/ : algorithms for finding shortest path in time depenedent graph.
	
```