#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "graph.h"

int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO < 0)) {
		std::cout << "SDL could not initialize. SDL Error: " << SDL_GetError() << '\n';
	}

	graph graph = {1, 20, 70, 40};

	const int SCREEN_WIDTH = graph.cell_size*graph.grid_width + 1;
	const int SCREEN_HEIGHT = graph.cell_size*graph.grid_height + 1;
	const int SCREEN_FPS = 60;
	const int SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;

	SDL_Window* window = SDL_CreateWindow("Graph implementation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		std::cout << "Window could not be initialized. SDL Error :" << SDL_GetError() << '\n';
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		std::cout << "Renderer could not be initialized. SDL Error :" << SDL_GetError() << '\n';
		return -1;
	}

	SDL_Color background_color = {22, 22, 22, 255};
	SDL_Color line_color = {44, 44, 44, 255};
	SDL_Color ghost_cursor_color = {44, 44, 44, 255};
	SDL_Color start_color = {0, 204, 0, 255};
	SDL_Color end_color = {204, 0, 0, 255};
	SDL_Color wall_color = {233, 233, 233, 255};
	SDL_Color path_color = {0, 0, 150, 255};

	SDL_Rect cursor = {0, 0, graph.cell_size, graph.cell_size};
	SDL_Rect wall = {-graph.cell_size, -graph.cell_size, graph.cell_size, graph.cell_size};
	SDL_Rect start_point = {-graph.cell_size, -graph.cell_size, graph.cell_size, graph.cell_size};
	SDL_Rect end_point = {-graph.cell_size, -graph.cell_size, graph.cell_size, graph.cell_size};
	SDL_Rect path = {-graph.cell_size, -graph.cell_size, graph.cell_size, graph.cell_size};

	std::vector<int> paths;

	Uint32 frame_start;
	Uint32 frame_time;

	bool quit = false;
	bool is_being_dragged = false;
	bool is_painting = false;
	bool start = false;
	bool end = false;
	bool run = false;
	bool draw_path = false;
	SDL_Event e;

	int last_point_x = 0, last_point_y = 0;

	while (!quit) {
		frame_start = SDL_GetTicks();
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym) {
				case SDLK_p: // toggle wall painting
					is_painting = is_painting ? false : true;
					break;
				case SDLK_r: // reset the grid
					for (int i = 0; i < graph.grid_arr.size(); i++) {
						for (int j = 0; j < graph.grid_arr[i].size(); j++) {
							graph.grid_arr[i][j] = 1;
						}
					}
					graph.init_list();
					start_point.x = -graph.cell_size;
					start_point.y = -graph.cell_size;
					end_point.x = -graph.cell_size;
					end_point.y = -graph.cell_size;
					if (paths.capacity() > 0) {
						paths.clear();
						paths.shrink_to_fit();
					}
					break;
				case SDLK_q: // start point
					if (paths.capacity() > 0) {
						paths.clear();
						paths.shrink_to_fit();
					}
					start = true;
					break;
				case SDLK_e: // end point
					if (paths.capacity() > 0) {
						paths.clear();
						paths.shrink_to_fit();
					}
					end = true;
					break;
				case SDLK_SPACE: // run the algorithm
					run = true;
					draw_path = false;
					break;
				}
				break;
			case SDL_MOUSEMOTION: // get cursor's current position
				cursor.x = (e.motion.x/graph.cell_size) * graph.cell_size;
				cursor.y = (e.motion.y/graph.cell_size) * graph.cell_size;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (paths.capacity() > 0) {
					paths.clear();
					paths.shrink_to_fit();
				}
				is_being_dragged = true;
				break;
			case SDL_MOUSEBUTTONUP:
				is_being_dragged = false;
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		// Set background color
		SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
		SDL_RenderClear(renderer);

		// Draw grid lines
		SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);
		// Horizontal lines
		for (int y = 0; y < SCREEN_HEIGHT; y += graph.cell_size) {
			SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
		}
		// Vertical lines
		for (int x = 0; x < SCREEN_WIDTH; x += graph.cell_size) {
			SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
		}

		// Draw ghost cursor
		SDL_SetRenderDrawColor(renderer, ghost_cursor_color.r, ghost_cursor_color.g, ghost_cursor_color.b, ghost_cursor_color.a);
		SDL_RenderFillRect(renderer, &cursor);

		int point_x = cursor.x/graph.cell_size; // cursor x position in grid's width
		int point_y = cursor.y/graph.cell_size; // cursor y position in grid's height

		bool is_inside = (point_x >= 0 && point_x < graph.grid_width) && (point_y >= 0 && point_y < graph.grid_height)
						&& (last_point_x >= 0 && last_point_x < graph.grid_width) && (last_point_y >= 0 && last_point_y < graph.grid_height);

		if (is_being_dragged) {

			if (is_inside) {

				int step_x = abs(last_point_x - point_x);
				int step_y = abs(last_point_y - point_y);

				if (step_x > step_y) {
					graph.draw_line_h(point_x, point_y, last_point_x, last_point_y, end_point, start_point, is_painting);
				} else {
					graph.draw_line_v(point_x, point_y, last_point_x, last_point_y, end_point, start_point, is_painting);
				}

			}
		}

		// Draw walls
		SDL_SetRenderDrawColor(renderer, wall_color.r, wall_color.g, wall_color.b, wall_color.a);
		for (int i = 0; i < graph.grid_arr.size(); i++) {
			for (int j = 0; j < graph.grid_arr[i].size(); j++) {
				if (graph.grid_arr[i][j] == 0) {
					wall.x = j*graph.cell_size;
					wall.y = i*graph.cell_size;
					SDL_RenderFillRect(renderer, &wall);
				}
			}
		}

		if (is_inside && start && graph.grid_arr[point_y][point_x] == 1) {
			start_point.x = cursor.x;
			start_point.y = cursor.y;
			start = false;
		}

		SDL_SetRenderDrawColor(renderer, start_color.r, start_color.g, start_color.b, start_color.a);
		SDL_RenderFillRect(renderer, &start_point);

		if (is_inside && end && graph.grid_arr[point_y][point_x] == 1) {
			end_point.x = cursor.x;
			end_point.y = cursor.y;
			end = false;
		}

		if (run && !draw_path && (start_point.x >= 0 && start_point.y >= 0) && (end_point.x >= 0 && end_point.y >= 0)) {
			if (paths.capacity() > 0) {
				paths.clear();
				paths.shrink_to_fit();
			}
			graph.dijkstra((start_point.y/graph.cell_size)*graph.grid_width + (start_point.x/graph.cell_size), (end_point.y/graph.cell_size)*graph.grid_width + (end_point.x/graph.cell_size), paths);
			run = false;
			draw_path = true;
		}

		if (draw_path) {
			for (const auto& path_point : paths) {
				path.y = path_point/graph.grid_width;
				path.x = path_point - path.y*graph.grid_width;
				path.x *= graph.cell_size;
				path.y *= graph.cell_size;
				SDL_SetRenderDrawColor(renderer, path_color.r, path_color.b, path_color.b, path_color.a);
				SDL_RenderFillRect(renderer, &path);
			}
		}

		SDL_SetRenderDrawColor(renderer, end_color.r, end_color.g, end_color.b, end_color.a);
		SDL_RenderFillRect(renderer, &end_point);

		SDL_RenderPresent(renderer);

		frame_time = SDL_GetTicks() - frame_start;

		last_point_x = point_x;
		last_point_y = point_y;

		if (frame_time < SCREEN_TICKS_PER_FRAME) {
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);
		}
	}


	SDL_Quit();

	return 0;
}
