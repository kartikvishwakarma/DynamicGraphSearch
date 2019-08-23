#include "Bidirectional.h"

bool Bidirectional::f_stop = false;
int Bidirectional::impromptu_count = 0;
int Bidirectional::time_length = 10080;

//int Bidirectional::vertexSize = 50000;

Object::Object(int size, int th) : Bidirectional(size,th)
{ 
	//f_stop = false;
	
	Q = new Queue[size]; 
	dist = new unordered_map<int,int>[size];
	parent = new unordered_map<int,int>[size];
	least_flat_time = new unordered_map<int,int>[size];
	visited = new unordered_map<int,char>[size];
	is_flat = new unordered_map<int,char>[size];
	closed = new unordered_map<int,bool>[size];
}	
	
Object::Object(const Object &obj)
{ 
	//Q = priority_queue<pr, vector<pr>, greater<pr> >(n.Q); 
}

Object& Object::operator=(const Object& obj)
{
	if(this != &obj){
		dist = obj.dist;
		parent = obj.parent;
		least_flat_time = obj.least_flat_time;
		visited = obj.visited;
		is_flat = obj.is_flat;
		//Q = obj.Q;
	}
	return *this;
}

int Object::closeSize(int id)
{
	return closed[id].size();
}

int Object::impromptuRendezous(int fid, int tid, int node, int dest)
{
	int fTime = dist[fid][node];
	int tTime = dist[tid][node];
	int arriveTime = dist[tid][dest];

	if(is_flat[tid][node] && (fTime >= least_flat_time[tid][node]) 
		&& (fTime <= tTime) )
	{
		for(int time = tTime; time >= least_flat_time[tid][node]; time--)
		{
			if(time == fTime)
			{
				impromptu_count++;
				return arriveTime;
			}
		}
	}

	else if(!(is_flat[tid][node]) && (fTime == tTime))
	{
		impromptu_count++;
		return arriveTime;
	}

	return inf;
}

int Object::forwardSearch(int src, int dest, int d_time, int id)
{
	cout << "running forwardSearch with threadID: " << id << endl;
	int s, u, w;
	int cont = inf;
	for(auto s : getSet())
	{
		dist[id][s] = inf;
		visited[id][s] = 'U';
	}

	dist[id][src] = d_time;
	parent[id][src] = -1;

	Q[id].push({dist[id][src],src});
	closed[id][src] = -1;

	while(!Q[id].empty())
	{
		u = Q[id].top().second;
		w = Q[id].top().first;

		visited[id][u] = 'f';
		Q[id].pop();

		if(f_stop)
			return inf;

		if(u == dest)
		{
			if(!f_stop)
				f_stop = true;

			return w;
		}

		if(w>=time_length)
			continue;

		for(auto &v : getGraph(u))
		{
			if(dist[id][v.first] > v.second[w])
			{
				dist[id][v.first] = v.second[w];
				parent[id][v.first] = u;
				closed[id][v.first] = true;
				//Q[id].push({dist[id][v.first],v.first});
			}
		}
	}
	return inf;
}

int Object::traceSearch(int src, int dest, int d_time, int a_time, int id)
{
	cout << "running traceSearch with threadID: " << id << endl;
	int s,u,w;
	int old_w, prev_w;
	int cont = inf;

	for(auto s : getSet())
	{
		visited[id][s] = 'U';
		dist[id][s] = -1;
	}

	dist[id][dest] = a_time;
	parent[id][dest] = -1;
	Q[id].push({0,dest});
	closed[id][dest] = true;

	while(!Q[id].empty())
	{
		u = Q[id].top().second;
		w = a_time - Q[id].top().first;
		old_w = w;
		visited[id][u] = 't';
		Q[id].pop();

		if(f_stop)
			return inf;

		if(visited[0][u] == 'f')
			cont = impromptuRendezous(0,id,u,dest);

		if(cont != inf)
			return cont;

		else if( (u == src) && (w == d_time))
		{
			if(!f_stop)
				f_stop = true;
			return a_time;

		}
		// if trace search arrive at source node after depature time.
		else if( (u == src) &&  (w>= d_time))
			return a_time;

		// if trace search arrive at source node before depature time.
		else if(w<d_time)
			return inf;

		for(auto &v : getReverseGraph(u))
		{
			bool path = true;
			while(v.second[w] == inf)
			{
				w--;
				if(w<0)
				{
					path = false;
					break;
				}
			}
			//in case of no mapping w is choose to largest time less than it
			prev_w = w - 1;

			if(visited[id][v.first]=='U' && path &&
				dist[id][v.first] < v.second[w])
			{
				dist[id][v.first] = v.second[w];
				parent[id][v.first] = u;
				closed[id][v.first] = true;
				Q[id].push({a_time-v.second[w], v.first});
				/*check if time series is flat region or not, if have flat  
				 region, maintain least flat time into another variable.*/
				while(v.second[prev_w] == inf)
				{
					prev_w -= 1;
					if(prev_w<=0){
						prev_w = 0;
						break;
					}
				}
				if(v.second[prev_w]+1 != v.second[w])
				{
					is_flat[id][v.first] = true;
					least_flat_time[id][v.first] = v.second[prev_w]+1;
				}
			}
			w = old_w;
		} 
	}
	return inf;
}

Bidirectional::Bidirectional(int s, int th)
{
	theta = th;
	size = s;
	vertexSize = vertex.size();

	LowerBound = new int* [vertexSize];
	for(int i=0; i<vertexSize; i++)
		LowerBound[i] = new int [vertexSize];

	/*UpperBound = new int* [vertexSize];
	for(int i=0; i<vertexSize; i++)
		UpperBound[i] = new int [vertexSize];*/



}

Bidirectional::~Bidirectional()
{
	for(int i=0; i<vertexSize; i++)
		delete (LowerBound[i]);
	delete (LowerBound);

	// for(int i=0; i<vertexSize; i++)
	// 	delete (UpperBound[i]);
	// delete (UpperBound);	
}

void Bidirectional::Insert(int edge, int src_id, int dest_id)
{
	if(tag_graph[src_id].size() == 0){
		tag_graph[src_id] = unordered_map<int, vector<int> > ();
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);

	}
	else if(tag_graph[src_id][dest_id].size() == 0){
		tag_graph[src_id][dest_id] = vector<int> (time_length, inf);
	}
	
	for(int i=1; i<gen.size(); i++){
		for(int p = gen[i-1].first; p < gen[i].first; p++){
			tag_graph[src_id][dest_id][p] = p + gen[i-1].second;
		}
	}	
	
	S.insert(src_id);
	S.insert(dest_id);

	return;
}

void Bidirectional::bidirectionalSearch(int src, int dest, int d_time, int LBT, int len, ofstream& out_trace)
{
	cout << "executing Bidirectional search\n";
	int o_id, i_id;
	int i, a_time;
	int min_ford;
	int min_time = time_length;
	int min_trace;
	int last_time = -1;
	int counter=0;
	int threshold = 7;
	int window_size = size-1;
	int theta = 20;

	omp_set_num_threads(3);
	omp_set_nested(1);

	#pragma omp parallel default(none) private(o_id) firstprivate(window_size,src, dest)\
	 shared(min_ford, min_trace, min_time, d_time, len, LBT,theta)
	{
		//printf("theadID: %d\n", omp_get_thread_num());
		o_id = omp_get_thread_num();

		if(o_id == 0)
			min_ford = forwardSearch(src,dest,d_time,o_id);
		
		else if(o_id == 1)
		{
			while( (impromptu_count < 1) && (f_stop == false));

			if(f_stop == false)
			{
				auto t1 = steady_clock::now();
				auto t2 = steady_clock::now();
				auto duration = duration_cast<milliseconds>(t2-t1).count();
				while(duration < theta)
				{				
					t2 = steady_clock::now();
					duration = duration_cast<milliseconds>(t2-t1).count();
				}
			}
			f_stop = true; 
		}

		else
		{
			omp_set_num_threads(window_size);
			min_trace = inf;
			#pragma omp parallel default(none) private(min_time, i_id,a_time) firstprivate(src, dest, d_time, LBT) shared(min_trace)
			{
				
				i_id = omp_get_thread_num();
				a_time = LBT + d_time + i_id ;
				min_time = traceSearch(src, dest, d_time, a_time, i_id+1);

				//#pragma omp barrier

				#pragma omp critical
				{
					min_trace = min(min_trace, min_time);
				}
				//printf("thread: %d a_time: %d min_time: %d min_trace: %d\n", i_id, a_time, min_time, min_trace);
			}
		}
	}

	/*out_trace << "src: " << to_string(src) << " depature at " << to_string(d_time)
			  << " arrival dest: " << to_string(dest) << " at " << to_string(min(min_trace, min_ford))
			  << " (min_ford: " << to_string(min_ford) << ", min_trace: " << to_string(min_trace) << " LBT: "
			  << to_string(LBT) << " len: " << to_string(len) << ") f_close: " <<to_string(closeSize(0))
			  <<" t_close: " << to_string(closeSize(1)) << endl;*/

	cout << "src: " << to_string(src) << " depature at " << to_string(d_time)
			  << " arrival dest: " << to_string(dest) << " at " << to_string(min(min_trace, min_ford))
			  << " (min_ford: " << to_string(min_ford) << ", min_trace: " << to_string(min_trace) << " LBT: "
			  << to_string(LBT) << " len: " << to_string(len) << ") f_close: " <<to_string(closeSize(0))
			  <<" t_close: " << to_string(closeSize(1)) << endl;		  

  	return;
}

void Bidirectional::readCase(string dir, string querydir, string outdir)
{
	cout << "executing readCase()" << endl;
	int dep_times[] = {0,450,540};//{0,540,600,1140,1200};
	//string patt = "(10000_src_dest_)(.*)";
	string patt = "(50000_src_dest_)(.*)"; 	
	string file;
	DIR *dp, *dp_out;
	struct dirent *ep;

	dp = opendir(querydir.c_str());

	if(dp == NULL)
	{
		perror("Failed to open Directory.");
		_Exit(123);
	}

	if(opendir(outdir.c_str()) == NULL) 
	{
		printf("Creating search Directory\n");
		const int dir_err = mkdir(outdir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
		    printf("Error creating directory: %s\n", outdir.c_str());
		    exit(1);
		}

	}

	ofstream out_trace;
	string name = "1st_impt10_"+to_string(theta)+".txt";
	
	out_trace.open(outdir+name);
	if(!out_trace.is_open())
	{
		printf("Error in open file: %s\n", name.c_str());
		_Exit(123);
	}

	while(ep = readdir(dp))
	{
		file = string(ep->d_name);
		if(regex_match(file, regex(patt)) )
		{
			//cout << file << endl;

			ifstream infile;
			infile.open(querydir+file);

			if(!infile.is_open())
			{
				printf("Error in open file: %s\n", ep->d_name);
				_Exit(102);
			}

			ofstream outfile;

			string line;
			int src, dest, len;

			stringstream ptr(file);
			string d_time, other, p_len;
			for(int i=0; i<3; i++)			
				getline(ptr, other, '_');
			getline(ptr, d_time, '_');
			getline(ptr, p_len, '.');
			//cout<< file << '\t' << d_time << ' ' << p_len << endl;

			int time = stoi(d_time);

			while(infile.good())
			{
				getline(infile, line, '\n');
				stringstream ss(line);

				ss >> src >> dest >> len;
				int LBT = LowerBound[vertex[src]][vertex[dest]];
				//int UBT = Upper_bound[vertex[src]][vertex[dest]];

				for(int k=0; k<3 && len>0; k++)
				{
					int d_time = dep_times[k];
					//string out = Out_dir + "sp_" + file;
					//string out = Out_dir + "par-sp_" + file;
					string out = outdir + "bi-dict_" + to_string(d_time) + "_" + file;
			
					outfile.open(out, ios_base::app);

			

					if(!outfile.is_open())
					{
						printf("Error in open file: %s\n", out.c_str());
						_Exit(101);
					}
						//if(len>=200){
						
					auto t1 = steady_clock::now();
					//printf("searching for src: %d dest: %d d_time: %d len: %d\n",
					//src, dest, time, len);
			
					bidirectionalSearch(src, dest, d_time, LBT,len, out_trace);
					auto t2 = steady_clock::now();
					auto duration = duration_cast<microseconds>(t2-t1).count();
					//cout << src << ' ' << dest << ' '<< duration << '\n';
			
					outfile << src << ' ' << dest << ' ' << duration << '\n';
					outfile.close();						
					//cout << src << ' ' << dest << ' ' << len << '\n';
					//}
					//Ptr p = Ptr(src, dest, true);
					}
			}
			infile.close();
		}
	}
	
}

void Bidirectional::createReverseGraph()
{
	cout << "creating reverse graph." << endl;
	for(auto &p : tag_graph){
        for(auto &l : p.second){
            vector<int> ptr = l.second;
            rev_tag_graph[l.first][p.first] = vector<int>(time_length,inf);
            for(int i=0; i<ptr.size(); i++){
                    if(ptr[i]<time_length)
                        rev_tag_graph[l.first][p.first][ptr[i]] = i;
          	}
        }
    }
}

void Bidirectional::createEarliestArrival()
{
	cout << "creating arrival time into earlist arrival time.\n";
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

void Bidirectional::readTimeSeries(string file, string infile)
{
	cout << "Getting timeseries information of graph\n";
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

	ifstream edgefile(infile);
	
	if(!edgefile.is_open())
	{
		cout << "Error: while open file: " << infile << endl;
		_Exit(1);
	}

	getline(edgefile, line, '\n');
	cout << "Reading " <<infile << " file with data format: \n";
	cout << line << '\n';

	// A flag for edege is twoWay or not.

	unordered_map<int,bool> twoway;

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
		printf("Error: while open file: %s\n", file.c_str());
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
			//graph[_src_id].insert({_dest_id, 1});


			while(tt >> attr){
				stringstream pp(attr);

				getline(pp, traveltime, ':');
				getline(pp, travelcost, ':');

				gen.push_back({stoi(traveltime), stoi(travelcost)});
				//cout << traveltime << ":" << travelcost << ' ';
			}
			//cout << '\n';

			gen.push_back({stoi(traveltime)+1, 0});
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

void Bidirectional::readBoundFile(string file, char type)
{
	cout << "Getting graph bounding information\n";
	ifstream infile;
	infile.open(file);

	if(!infile.is_open())
	{
		cerr << "Error in open file: " << file << endl;
		_Exit(123); 	
	}

	int row=0;
	int col;
	int data;

	string line;

	if(type == 'l')
	{
		while(infile.good())
		{
			getline(infile, line, '\n');
			stringstream ss(line);
			if(line.length())
			{
				for(col=0; col < vertexSize; col++){
					ss >> data;			
					LowerBound[row][col] =  data;		
							
				}
				row += 1;
			}			
		}
	}
	else if(type == 'u')
	{
		while(infile.good())
		{
			getline(infile, line, '\n');
			stringstream ss(line);
			if(line.length())
			{
				for(col=0; col < vertexSize; col++){
					ss >> data;			
					//Upper_bound[row][col] =  data;		
				}
				row += 1;
			}			
		}
	}
	return;	
}

void Bidirectional::getVertex(string file)
{
	cout << "Getting graph vertexs.";
	int row = 0;
	ifstream infile;
	infile.open(file);
	string line;
	if(!infile.is_open())
	{
		cerr << "Error in open file: " << file << endl;
		_Exit(123); 
	}
	while(infile.good())
	{
		getline(infile, line, '\n');		
		if(line.length()){
			vertex.insert({stoi(line), row});
			row += 1;
		}
		//cout << line << endl;
	}
	return;
}

void Object::showPath(int dest, int id)
{
	if(dest != -1){
		printf("%d --> ", dest);
		showPath(parent[id][dest], id);
	}
}

void Object::showRevPath(int dest, int id)
{
	if(dest != -1){
		printf("%d --> ", dest);
		showRevPath(parent[id][dest], id);
	}
}

void Bidirectional::showTAGraph()
{
	cout << "SP TAG Graph: " << endl;
	for(auto &p : tag_graph)
	{
		//cout << p.first << " | ";
		for(auto &l : p.second)
		{
			cout << p.first << " | " << " --> " << l.first << " { ";
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

void Bidirectional::showRevTAGrap()
{
	for(auto &p : rev_tag_graph)
	{
        for(auto &l : p.second)
        {
            cout<< p.first << " | " << " --> " << l.first << " { ";
            vector<int> v = l.second;
            for(int i=0; i<time_length; i++)
                    if(v[i]<inf)
                            cout << i<<':' << v[i] << ' ';
            cout << "}\n";
        }
        cout << '\n';
    }
    return;
}
