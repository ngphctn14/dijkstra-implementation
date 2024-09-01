#pragma once

#include "graph.h"

graph::graph(int _scalar, int _cell_size, int _grid_width, int _grid_height) {
	scalar = _scalar;
	cell_size = _cell_size * _scalar;
	grid_width = _grid_width;
	grid_height = _grid_height;
	nodes = _grid_height * _grid_width;
	grid_arr.resize(_grid_height, std::vector<int>(_grid_width, 1));
	init_list();
}

void graph::generate_maze() {

}

void graph::dijkstra(int src, int dest, std::vector<int>& path) {
	if (src == dest) {
		return;
	}

	std::vector<int> dist(nodes, INT_MAX);
	std::vector<int> prev(nodes, -1);
	std::vector<bool> visited(nodes, false);

	dist[src] = 0;

	while (!visited[dest]) {
		int v = min_dist(dist, visited);

		if (dist[v] == INT_MAX) {
			return;
		}

		visited[v] = true;

		for (int x : adj_list[v]) {
			if (dist[v] + 1 < dist[x]) {
				dist[x] = dist[v] + 1;
				prev[x] = v;
			}
		}
	}

	int u = dest;

	while (prev[u] != src) {
		u = prev[u];
		path.push_back(u);
	}
}

int graph::min_dist(const std::vector<int>& dist, const std::vector<bool>& visited) {
	int min = INT_MAX;
	int min_index = -1;
	for (int i = 0; i < nodes; i++) {
		if (dist[i] <= min && !visited[i]) {
			min = dist[i];
			min_index = i;
		}
	}
	return min_index;
}

void graph::init_list() {
	if (adj_list.capacity() > 0) {
		adj_list.clear();
		adj_list.shrink_to_fit();
	}
	adj_list.resize(nodes);
	for (int i = 0; i < grid_arr.size(); i++) {
		for (int j = 0; j < grid_arr[i].size(); j++) {
			// up
			if (i - 1 >= 0 && grid_arr[i-1][j] == 1) {
				int up_i = i - 1;
				add_edge(i*grid_width + j, up_i*grid_width + j);
			}
			// left
			if (j - 1 >= 0 && grid_arr[i][j-1] == 1) {
				int left_j = j - 1;
				add_edge(i*grid_width + j, i*grid_width + left_j);
			}
			// right
			if (j + 1 < grid_arr[i].size() && grid_arr[i][j+1] == 1) {
				int right_j = j + 1;
				add_edge(i*grid_width + j, i*grid_width + right_j);
			}
			// down
			if (i + 1 < grid_arr.size() && grid_arr[i+1][j] == 1) {
				int down_i = i + 1;
				add_edge(i*grid_width + j, down_i*grid_width + j);
			}
		}
	}
}

void graph::add_edge(int u, int v) {
	adj_list[u].push_back(v);
}

void graph::add_undirected_edge(int u, int v) {
	adj_list[u].push_back(v);
	adj_list[v].push_back(u);
}

void graph::remove_edge(int u, int v) {
	for (int i = 0; i < adj_list[u].size(); i++) {
		if (adj_list[u][i] == v) {
			adj_list[u].erase(adj_list[u].begin() + i);
			break;
		}
	}
	for (int i = 0; i < adj_list[v].size(); i++) {
		if (adj_list[v][i] == u) {
			adj_list[v].erase(adj_list[v].begin() + i);
			break;
		}
	}
}

void graph::add_node(int u) {
	int j = u % grid_width;
	int i = (u - j)/grid_width;
	// up
	if (i - 1 >= 0 && grid_arr[i-1][j] == 1) {
		int up_i = i - 1;
		add_undirected_edge(i*grid_width + j, up_i*grid_width + j);
	}
	// left
	if (j - 1 >= 0 && grid_arr[i][j-1] == 1) {
		int left_j = j - 1;
		add_undirected_edge(i*grid_width + j, i*grid_width + left_j);
	}
	// right
	if (j + 1 < grid_arr[i].size() && grid_arr[i][j+1] == 1) {
		int right_j = j + 1;
		add_undirected_edge(i*grid_width + j, i*grid_width + right_j);
	}
	// down
	if (i + 1 < grid_arr.size() && grid_arr[i+1][j] == 1) {
		int down_i = i + 1;
		add_undirected_edge(i*grid_width + j, down_i*grid_width + j);
	}
}

void graph::remove_node(int u) {
	adj_list[u].clear();
	adj_list[u].shrink_to_fit();
	int j = u % grid_width;
	int i = (u - j)/grid_width;
	// up
	if (i - 1 >= 0 && grid_arr[i-1][j] == 1) {
		int up_i = i - 1;
		remove_edge(i*grid_width + j, up_i*grid_width + j);
	}
	// left
	if (j - 1 >= 0 && grid_arr[i][j-1] == 1) {
		int left_j = j - 1;
		remove_edge(i*grid_width + j, i*grid_width + left_j);
	}
	// right
	if (j + 1 < grid_arr[i].size() && grid_arr[i][j+1] == 1) {
		int right_j = j + 1;
		remove_edge(i*grid_width + j, i*grid_width + right_j);
	}
	// down
	if (i + 1 < grid_arr.size() && grid_arr[i+1][j] == 1) {
		int down_i = i + 1;
		remove_edge(i*grid_width + j, down_i*grid_width + j);
	}
}
