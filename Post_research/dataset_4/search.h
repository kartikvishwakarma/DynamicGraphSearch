#ifndef SEARCH_H
#define SEARCH_H

const int no_of_vertex = 50000;

void dist_SP_TAG(int src, int dest, int time, int threads, int len){
	//printf("executing dist_SP_TAG with %d %d\n", src,dest);
	int my_id, nth, size;
	int my_start, my_end;
	int my_u, my_v, my_w;
	int min = inf;
	int u, w=inf;
	int min_id,pos;
	int range;
	pr edge;

	bool empty = false;
	bool my_empty;
	vect m;
	vect::iterator p;
	vector<int> dist;

	unordered_map<int,int> ptr;

	int no = 0;

	for(auto s : S){
		//cout << s << ' ';
		visited[s] = false;
		ptr[s] = no;
		dist.push_back(inf);
		no += 1;
	}
	//S.clear();

	int N = dist.size();
	MinHeap Q = MinHeap();
	//printf("dist size: %d  | threads: %d\n",N, threads);
	omp_set_num_threads(threads);
	#pragma omp parallel default(none) firstprivate(ptr) private(Q, pos, my_start, my_end, my_id, my_u, my_w, p, my_empty, edge, nth,range) \
	 shared(time, min_id, size, m, dist, u, w, src, N, empty,tag_graph,dest,len)
	{

		my_id = omp_get_thread_num();
		nth = omp_get_num_threads();
		//printf("my_id: %d nth: %d\n", my_id, nth);
		my_start = my_id * N / nth;
		my_end = (my_id+1) * N / nth - 1;

		pos = ptr[src];
		//range = pos/nth;
		if(pos >= my_start && pos <= my_end){
			dist[pos] = time;
			Q.push({src, time});
		}

		while(!empty){
			//printf("inside while\n");
			#pragma omp barrier

			#pragma omp single
			{
				u = inf;
				w = inf;
				empty = true;
			}
			#pragma omp barrier

			edge = Q.top();
			my_u = edge.first;
			my_w = edge.second;
			//printf("my_id:%d my_u:%d my_w:%d\n", my_id,my_u,my_w);
			#pragma omp barrier

			#pragma omp critical
			{
			if(w > my_w){
				w = my_w;
				u = my_u;
				min_id = my_id;
			}
			}
			
			#pragma omp barrier
			//printf("min_id:%d u:%d w:%d\n",min_id,u,w);
			

			#pragma omp single
			{
			if( u == dest){
				printf("%d %d %d\n", time, len ,w);
				empty = true;
			}
			
			m = tag_graph[u];
			size = m.size();
			}

			#pragma omp barrier

			if(w != inf && min_id == my_id)
				Q.pop();

			for(p = m.begin(); p != m.end(); p++){
				pos = ptr[p->first];
				//printf("pos:%d u:%d v:%d w:%d dist:%d\n", pos, u,p->first,w,dist[pos]);
				if(pos >= my_start && pos <= my_end){ 
					if(dist[pos] > p->second[w]){
						dist[pos] = p->second[w];
						//printf("u:%d w:%d\n", p->first, dist[pos]);
						Q.push({p->first, dist[pos]});
					}
				}
			}

			#pragma omp critical
			{
				empty = empty & Q.empty();
			}

			#pragma omp barrier
		}
	}
	
	return;
}

int Object::parallel_SP_TAG(int src, int dest, int time, int len){
	//printf("par-sp with src:%d dest:%d\n",src,dest);
	Queue Q;
	Map dist;
	int s, w, n, val, u,i;
	int id;
	
	for(auto s : S){
		dist[s] = inf;
		visited[s] = false;
	}
	//S.clear();

	dist[src] = time;
	
	Q.push({dist[src], src});
	
	vect::iterator it;
	//link ptr;
	
	while(!Q.empty()){
		s = Q.top().second;
		w = Q.top().first;
		Q.pop();

		if(s == dest){
			//printf("arrive at %d\n\n", w);
			printf("%d %d %d\n", time, len ,w);
			return w;
		}
		
		n = tag_graph[s].size();
		
		if(n){
			//vector<int> ptr = mapper[s];
			#pragma omp parallel num_threads(n) firstprivate(n,s,w) private(i,val,it, id) shared(Q, dist, tag_graph, mapper)
			{
			//printf("inside parallel region\n");
			id = omp_get_thread_num();
			it = tag_graph[s].begin();
			advance(it, id);
			
			
			//printf("before if w: %d\n", w);
			
			if(dist[it->first] > it->second[w]){
				#pragma omp critical
				{
					dist[it->first] = it->second[w]; // + p.second;
					Q.push({it->second[w], it->first});	
				}
			
			//printf("after if w: %d\n\n", w);
			}
			}
		}
	}

	return inf;
}


int Object::Forward_search(int src, int dest, int time, int len, ofstream& out_trace){
	//printf("\n\n(thread: %d) executing Forward_search with depature time: %d\n\n",id, time);
	Queue Q;
	
	int s, u, w;
	int cont = inf;
	unordered_map<int,bool> closed;

	for(auto s : S){
		dist[s] = inf;
		visited[s] = 'U';
	}
	//S.clear();

	dist[src] = time;
	parent[src] = -1;
	
	Q.push({dist[src], src});
	closed[src] = true;
		
	while(!Q.empty()){
		u = Q.top().second;
		w = Q.top().first;
		visited[u] = 'f';

		Q.pop();
		//for(int i=1; i<= limit; i++){
		//usleep(1000);
		//printf("reach to node: %d at %d\n",u,w);

		if(u == dest){
			//printf("Forward_search set f_stop\n");
			//printf("arrival time at %d is %d from %d depature at %d\n\n", dest, w, src, time);
			//printf("\n");
			//Show_dist(src);
			out_trace << time << ' ' << len << ' ' << w << ' ' << closed.size() << '\n';			
			//printf("%d %d %d %ld\n", time, len ,w, closed.size());
			return w;
		}

		if(w>=time_length){
			//printf("for node: %d exceeds time series,\n...skipping its updation...\n", u);
			continue;
		}

		for(auto &v : tag_graph[u]){
			if(dist[v.first] > v.second[w]){
				dist[v.first] = v.second[w];
				parent[v.first] = u;
				closed[v.first] = true;
				Q.push({dist[v.first], v.first});
			}
		}
		
		//Show_dist(src);
	}
	return inf;
}

void Object::Show_path(int dest, int id){
	if(dest != -1){
		printf("%d --> ", dest);
		Show_path(parent[dest], id);
	}		
}

void Readfile(string file){
	string line, x_coord, y_coord, attr;
	string edge_id, src_id, dest_id, dist;
	string time_series, traveltime, travelcost;
	int _edge_id, _src_id, _dest_id;
	/*
		Read Edge.csv file and store { edge_id, src_id, dest_id }
		information into main memory.

		Now Read timeseries.txt file for spatial temporal dataset
		and for respective edge_id, assign travel cost to 
		corresponsing source , destination pair.  
	*/
	string str = "Edges_" + to_string(no_of_vertex) + ".csv";
	ifstream edgefile(str);

	if(!edgefile.is_open()){
		cout << "Error: while open file " << str << "\n";
		_Exit(1);
	}

	getline(edgefile, line, '\n');
	printf("Reading 'Edegs.csv' file with data format: \n");
	cout << line << '\n';

	// A flag for edege is twoWay or not.

	Mark twoway;

	while(edgefile.good()){
		getline(edgefile, x_coord, ',');
		getline(edgefile, y_coord, ',');
		getline(edgefile, src_id, ',');
		getline(edgefile, dest_id, ',');
		getline(edgefile, edge_id, ',');
		getline(edgefile, dist, '\n');

		_edge_id = stoi(edge_id);

		if(edgeinfo.find(_edge_id) != edgeinfo.end() )
			twoway[_edge_id] = true;
		
		edgeinfo[_edge_id] = {stoi(src_id), stoi(dest_id)};

	}
	//Show_edgeinfo();
	edgefile.close();
	
	/*  
		Read timeseries.txt file a time series dataset
		file contain edge_id and time series : cost

		based on edge_id , it get src_id, dest_id 
		and create time aggregrate graph for time series: cost
		from file "timeseries.txt".
	 */

	
	ifstream timeseriesfile(file);


	if(!timeseriesfile.is_open()){
		printf("Error: while open timeseries.txt file \n");
		_Exit(1);
	}

	while(timeseriesfile.good()){
		getline(timeseriesfile, line, '\n');

		if(line.length()){
			stringstream ss(line);
			getline(ss, edge_id, '|');
			getline(ss, time_series, '|');

			stringstream tt(time_series);
			_edge_id = stoi(edge_id);
			_src_id = edgeinfo[_edge_id].first;
			_dest_id = edgeinfo[_edge_id].second;
			//cout << _src_id <<' '<< _dest_id << '\n';
			graph[_src_id].insert({_dest_id, 1});


			while(tt >> attr){
				stringstream pp(attr);

				getline(pp, traveltime, ':');
				getline(pp, travelcost, ':');

				gen.push_back({stoi(traveltime), stoi(travelcost)});
			}

			//cout << edge_id << ' ' << edgeinfo[_edge_id].first << ' ' << edgeinfo[_edge_id].second<< '\n';
			Insert(_edge_id, _src_id, _dest_id);
			
			if(twoway[_edge_id])
				Insert(_edge_id, _dest_id, _src_id);

			gen.clear();
			 
		}
	}
	twoway.clear();
	edgeinfo.clear();
	timeseriesfile.close();	
	
}

void Insert(int _edge_id, int src_id, int dest_id){
	//printf("call Insert with %d %d %d\n", _edge_id, src_id, dest_id);
	if(tag_graph[src_id].size() == 0){
		tag_graph[src_id] = unordered_map<int, vector<int> > ();
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);

	}
	else if(tag_graph[src_id][dest_id].size() == 0){
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);
	}

	for(int i = 1; i < gen.size(); i++){
		for(int p = gen[i-1].first; p < gen[i].first; p++){
			tag_graph[src_id][dest_id][p] = p + gen[i-1].second;
		}
	}	

	S.insert(src_id);
	S.insert(dest_id);
	
	//mapper[src_id].push_back(dest_id);

	return;
}

void Create_ealiest_arrival(){
	for(auto &p : tag_graph){
		for(auto &l : p.second){
			int Min = inf;
			for(int k = time_length-1; k>=0;k--){
				Min = min(Min, l.second[k]);
				if(Min<l.second[k])
					l.second[k] = Min;
			}
		}
	}
}

void Show_tag_graph(){
	for(auto &p : tag_graph){
		//cout << p.first << " | ";
		for(auto &l : p.second){
			cout << node[p.first] << " | " << " --> " <<node[l.first] << " { ";
			vector<int> ptr = l.second;
			for(int i=0; i<ptr.size(); i++)
				if(ptr[i] < inf)
					cout << i <<':' << ptr[i] << ' ';
			cout << "}\n";
		}

		cout << '\n';
	}
	return;
}

#endif
