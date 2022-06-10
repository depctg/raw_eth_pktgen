#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <chrono>

#define MAX_V 30000
#define NO_EDGE 0.0
#define at(G, r, c) (G[r * (MAX_V) + c])
#define min_d(x, y) ((x) < (y) ? (x) : (y))

using namespace std;

constexpr static double MAX_D = numeric_limits<double>::max();

int init_weight(const char *fpath, double *G)
{
    ifstream f(fpath);
    if (!f.is_open())
    {
        cout << "Failed to open " << fpath << endl;
    }
    int eid, sid, tid, vid_max;
    eid = sid = tid = 0;
    vid_max = -1;
    double w;
    string line;
    while (getline(f, line))
    {
        istringstream iss(line);
        iss >> eid >> sid >> tid >> w;
        if (sid < MAX_V && tid < MAX_V)
        {
            at(G, sid, tid) = w;
            // undirected
            if (at(G, tid, sid) == NO_EDGE)
                at(G, tid, sid) = w;
            else if (at(G, tid, sid) != w)
                cout << "Not sym graph" << endl;
            vid_max = (vid_max > sid) ? vid_max : sid;
            vid_max = (vid_max > tid) ? vid_max : tid;
        }
        // cout << eid << " " << sid << " " << tid << " " << w << endl;
    }
    f.close();
    return vid_max;
}

int min_dist(double *s_D_t, bool *not_visited, int V)
{
    double min_dist = MAX_D;
    int min_idx;
    for (int i = 0; i < V; ++i)
        if (not_visited[i] && s_D_t[i] < min_dist)
            min_dist = s_D_t[i], min_idx = i;
    return min_idx;
}

void dijkstra(double *G, int src, double *dist, int V) 
{
    bool not_visited[V];
    for (int i = 0; i < V; ++i)
        dist[i] = MAX_D, not_visited[i] = true;
    dist[src] = 0.0;
    for (int c = 0; c < V - 1; ++c) 
    {
        int t = min_dist(dist, not_visited, V);
        not_visited[t] = false;
        // update adjacent v dist
        if (dist[t] >= MAX_D) continue;
        for (int adj_i = 0; adj_i < V; ++ adj_i)
            if (not_visited[adj_i] && at(G, t, adj_i) != NO_EDGE)
                dist[adj_i] = min_d(dist[adj_i], dist[t] + at(G, t, adj_i));
    }
}

int main(int argc, char **argv)
{
    const char *fpath = argv[1];
    double *graph;
    if (posix_memalign((void **)&graph, 64, MAX_V * MAX_V * sizeof(double)))
    {
        cout << "Allocating memory failed" << endl;
        exit(EXIT_FAILURE);
    }
    for (uint64_t i = 0; i < MAX_V * MAX_V; ++i)
        graph[i] = NO_EDGE;

    int total_v = init_weight(fpath, graph);
    cout << "Total number of vertices: " << total_v << endl;

    // 10875 10643 10644 0.002981
    // cout << at(graph, 10643, 10644) << endl;

    double *solution = new double[total_v];
    dijkstra(graph, 0, solution, total_v);

    ofstream output("path_M.txt");
    for (int i = 0; i < total_v; ++ i)
        output << i << ", " << solution[i] << endl;
    output.close();
    return 0;
}