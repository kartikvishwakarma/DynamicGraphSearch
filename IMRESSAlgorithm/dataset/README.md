#Extraction graph information.

Available graph dataset are extracted from  "QGIS sortware" which contain many file type including csv, cpg, dbf, prj, sbn, sbx, shp, shx, cpg.

But for searching in graph only nodes, edges required.

For this purpose, this code extract node and edge information and put them in saperate file, names as: 
"Node_<size>.cvs" and "Edge_<size>.csv".

Availabel dataset is a static graph, so we generated time varying graph synthetically and kept in another separate file named as 
"timeseries_<size>.txt". 