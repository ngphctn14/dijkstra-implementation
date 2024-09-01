#include <vector>
#include <iostream>
#include <limits.h>
#include <algorithm>
#include <random>

#include <SDL2/SDL.h>

struct graph {
	int scalar;
	int cell_size;
	int grid_width;
	int grid_height;
	int nodes;

	std::vector<std::vector<int>> adj_list;
	std::vector<std::vector<int>> grid_arr;

	graph(int _scalar, int _cell_size, int _grid_width, int _grid_height);
	void generate_maze();
	void add_edge(int u, int v);
	void add_undirected_edge(int u, int v);
	void remove_node(int u);
	void add_node(int u);
	void remove_edge(int u, int v);
	void init_list();
	int min_dist(const std::vector<int>& dist, const std::vector<bool>& visited);
	void dijkstra(int src, int dest, std::vector<int>& path);
};
