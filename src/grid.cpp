#include "grid.hpp"
#include "point.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>

Grid::Grid() {
#ifdef DEBUG
    std::cout << "---- INIT Grid ----" << std::endl;
#endif
    this->width = this->right_limit - this->left_limit;
    this->height = this->top_limit - this->bottom_limit;

    this->max_cells_count = 20;

    // Determine the number of horizontal/vertical cells
    if (this->height >= this->width) {
        this->horizontal_cells = static_cast<int>(
            (this->width / this->height) * this->max_cells_count); 
        this->vertical_cells = this->max_cells_count;
    } else {
        this->vertical_cells = static_cast<int>(
            (this->height / this->width) * this->max_cells_count); 
        this->horizontal_cells = this->max_cells_count;
    }

    // Determine the width and height of every cell
    this->cell_width = this->width / (double) this->horizontal_cells;
    this->cell_height = this->height / (double) this->vertical_cells;

    // Create matrix full of 0
    for (int j = 0; j < vertical_cells; j++) {
        std::vector<bool> temp_vec;
        for (int i = 0; i < horizontal_cells; i++) {
            temp_vec.push_back(false);
        }
        this->matrix.push_back(temp_vec);
    }
}

bool Grid::in_grid(glm::ivec2 cell) {
    bool condition1 = cell.x >= 0 && cell.x < horizontal_cells; 
    bool condition2 = cell.y >= 0 && cell.y < vertical_cells;
    bool flag = false;
    if (condition1 && condition2) {
        flag = true;
    }
    return flag; 
}

// Manhattan Distance between two cells
int Grid::get_distance(glm::ivec2 c1, glm::ivec2 c2) {
    return std::abs(c1.x - c2.x) + std::abs(c1.y - c2.y);
}

void Grid::populate(std::vector<glm::vec2> all_points) {
    this->all_points = all_points;

    if (all_points.size() > 0) {
        for (auto point : all_points) {
            auto coordinates = this->get_coordinates(point.x, point.y);
            this->matrix[coordinates.y][coordinates.x] = true;
        }
    }
}

glm::ivec2 Grid::get_coordinates(double x, double y) {
    // std::cout << "Grid: get_coordinates" << std::endl;

    double x_diff = x - this->left_limit; 
    double y_diff = this->top_limit - y;

    // std::cout << "x_diff " << x_diff << std::endl;
    // std::cout << "y_diff " << y_diff << std::endl;

    // Determine the coordinates on the grid
    int x_pos = static_cast<int>(floor(x_diff / this->cell_width));
    int y_pos = static_cast<int>(floor(y_diff / this->cell_height));

    if (x_pos < 0) {
        x_pos = 0;
    }

    // std::cout << "x_pos " << x_pos << std::endl;
    // std::cout << "y_pos " << y_pos << std::endl;

    auto coordinates = glm::ivec2(x_pos, y_pos); 
    return coordinates;
}

void Grid::clear() {
    for (int j = 0; j < this->vertical_cells; j++) {
        for (int i = 0; i < this->horizontal_cells; i++) {
            this->matrix[j][i] = false;
        }
    }
}

int Grid::direction_to_cuadrant(glm::vec2 direction) {
    double x_diff = direction.x;
    double y_diff = direction.y;

    glm::vec2 vec1 {1, 0};
    glm::vec2 vec2 {x_diff, y_diff};
    auto rotation = cross_product(vec1, vec2);
    double angle;

    if (rotation > 0) {
        angle = get_angle(vec1, vec2);
    } else if (rotation < 0) {
        angle = 2 * M_PI - get_angle(vec1, vec2);
    } else {
        angle = 0;
    }

    angle = angle * 180 / M_PI;

    int cuadrant = 0;
    double last_angle = 45 / 2;

    for (int i = 1; i < 8; i++) {
        if (angle >= (360 - (45 / 2)) || angle < (45 / 2)) {
            cuadrant = 0;
            break;
        }

        if (angle >= (last_angle) && angle < (last_angle + 45)) {
            cuadrant = i;
            break;
        } 

        last_angle += 45;
    }
    
    return cuadrant;
}

void Grid::check_cuadrants(int direction_cuadrant) {
    std::vector<glm::ivec2> neighbours;

    /*  Grid Notation for a pair of coordinates c
        from 0 to 1 neighbours of the center
        |3|2|1|
        |4|c|0|
        |5|6|7|
    */

    // Neighbour 0
    neighbours.push_back(glm::ivec2(center.x + 1, center.y));

    // Neighbour 1
    neighbours.push_back(glm::ivec2(center.x + 1, center.y + 1));

    // Neighbour 2
    neighbours.push_back(glm::ivec2(center.x, center.y + 1));

    // Neighbour 3
    neighbours.push_back(glm::ivec2(center.x - 1, center.y + 1));

    // Neighbour 4
    neighbours.push_back(glm::ivec2(center.x - 1, center.y));

    // Neighbour 5
    neighbours.push_back(glm::ivec2(center.x - 1, center.y - 1));

    // Neighbour 6
    neighbours.push_back(glm::ivec2(center.x, center.y - 1));

    // Neighbour 7
    neighbours.push_back(glm::ivec2(center.x + 1, center.y - 1));

    // Make every possible neigbour invalid
    for (auto neighbour : neighbours) {
        if (this->in_grid(neighbour) == true) {
            this->matrix[neighbour.y][neighbour.x] = false;
        }
    }

    /*  Check which neighbours are valid. It depends of the movement 
        constraints of the player
        |-|^|-|      |x|x|x|  
        |-|c|-| -->  |-|c|-|
        |-|-|-|      |-|-|-|

        |-|-|-|      |-|-|x|  
        |-|c|>| -->  |-|c|x|
        |-|-|-|      |-|-|x|
    */

    std::vector<glm::ivec2> temp_neighbours;
    std::vector<int> valid_id;
    if (direction_cuadrant == 0) {
        temp_neighbours.push_back(neighbours[1]);
        temp_neighbours.push_back(neighbours[0]);
        temp_neighbours.push_back(neighbours[7]);
    } else if (direction_cuadrant == 7) {
        temp_neighbours.push_back(neighbours[0]);
        temp_neighbours.push_back(neighbours[7]);
        temp_neighbours.push_back(neighbours[6]);
    } else {
        temp_neighbours.push_back(neighbours[direction_cuadrant + 1]);
        temp_neighbours.push_back(neighbours[direction_cuadrant]);
        temp_neighbours.push_back(neighbours[direction_cuadrant - 1]);
    }

    // Check that the neighbours are in limit of the grid
    std::vector<glm::ivec2> valid_neighbours;
    for (auto neighbour : temp_neighbours) {
        if (this->in_grid(neighbour) == true) {
            this->matrix[neighbour.y][neighbour.x] = true;
        }
    }
}

void Grid::set_player(glm::vec2 center, glm::vec2 direction) {
    this->clear();
    this->populate(this->all_points);
    this->center = this->get_coordinates(center.x, center.y);
    int direction_cuadrant = this->direction_to_cuadrant(direction);
    this->check_cuadrants(direction_cuadrant);
}

std::vector<glm::ivec2> Grid::get_neighbours(glm::ivec2 cell) {
    std::vector<glm::ivec2> neighbours;

    // Neighbour 0
    neighbours.push_back(glm::ivec2(center.x + 1, center.y));

    // Neighbour 1
    neighbours.push_back(glm::ivec2(center.x + 1, center.y + 1));

    // Neighbour 2
    neighbours.push_back(glm::ivec2(center.x, center.y + 1));

    // Neighbour 3
    neighbours.push_back(glm::ivec2(center.x - 1, center.y + 1));

    // Neighbour 4
    neighbours.push_back(glm::ivec2(center.x - 1, center.y));

    // Neighbour 5
    neighbours.push_back(glm::ivec2(center.x - 1, center.y - 1));

    // Neighbour 6
    neighbours.push_back(glm::ivec2(center.x, center.y - 1));

    // Neighbour 7
    neighbours.push_back(glm::ivec2(center.x + 1, center.y - 1));

    std::vector<glm::ivec2> valid_neighbours;
    for (auto neighbour : neighbours) {
        if (this->in_grid(neighbour) == true) {
            if (this->matrix[neighbour.y][neighbour.x] == true) {
                valid_neighbours.push_back(neighbour);
            }            
        }
    }

    return valid_neighbours;
}

glm::ivec2 Grid::get_next_cell(glm::ivec2 start, glm::ivec2 goal) {
    auto start_ptr = std::make_shared<glm::ivec2>(start);

    // std::priority_queue<std::pair<int, std::shared_ptr<glm::ivec2>>> frontier;
    PriorityQueue<std::shared_ptr<glm::ivec2>, int> frontier;
    frontier.put(start_ptr, 0);

    std::map<std::shared_ptr<glm::ivec2>, std::shared_ptr<glm::ivec2>> came_from;
    std::map<std::shared_ptr<glm::ivec2>, int> cost_so_far;

    came_from.insert({start_ptr, nullptr});
    // came_from.insert({start_ptr, std::make_shared<glm::ivec2>(-1, -1)});
    cost_so_far.insert({start_ptr, 0});
    
    while (!frontier.empty()) {
        auto current = frontier.get();
        
        std::cout << "Start: " << start.x << "," << start.y << std::endl;
        std::cout << "Current: " << current->x << "," << current->y << std::endl;
        std::cout << "Goal: " << goal.x << "," << goal.y << std::endl;
        
        if (current->x == goal.x && current->y == goal.y) {
            break;
        }

        auto neighbours = this->get_neighbours(*current);
        bool has_more = false;
        for (auto next : neighbours) {
            if (next.x != current->x && next.y != current->y) {
                std::cout << "Next: " << next.x << "," << next.y << std::endl;
                auto next_ptr = std::make_shared<glm::ivec2>(next);
                auto new_cost = cost_so_far.at(current);
                cost_so_far.insert({next_ptr, new_cost});
                auto priority = new_cost + this->get_distance(goal, next);
                frontier.put(next_ptr, priority);

                came_from.insert({next_ptr, current});
                has_more = true;
            }
        }

        std::cout << "----" << std::endl;
        if (has_more == false) {
            break;
        }

    }

    std::shared_ptr<glm::ivec2> next_cell_ptr;
    for (auto item : came_from) {
        auto cell_ptr = item.first;
        auto origin_ptr = item.second;
        if (origin_ptr == start_ptr) {
            next_cell_ptr = cell_ptr;
            break;
        }
    }

    glm::ivec2 next_cell;
    if (next_cell_ptr == nullptr) {
        next_cell = glm::ivec2(-1, -1);
    } else {
        next_cell = *next_cell_ptr;
    }

    return next_cell;
}

int Grid::get_new_direction(glm::vec2 center, glm::vec2 next_cell, glm::vec2 direction) {
    glm::vec2 diff_vec = next_cell - center;
    auto temp_direction = cross_product(diff_vec, direction);

    std::cout << "temp_direction: " << temp_direction << std::endl;

    int new_direction = 0;
    if (temp_direction < 0) {
        new_direction = -1;
    } else if (temp_direction > 0) {
        new_direction = 1;
    }
    
    return new_direction;
}

void Grid::print() {
    std::cout << std::endl;

    for (int j = 0; j < this->vertical_cells; j++) {
        for (int i = 0; i < this->horizontal_cells; i++) {
            glm::ivec2 position {i, j};
            bool value = this->matrix[j][i];
            if (value) {
                std::cout << "\033[1;31m" << value << "\033[0m";
            } else {
                std::cout << value;
            }
        }
        std::cout << std::endl;
    }

}