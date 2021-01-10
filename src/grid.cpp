#include "grid.hpp"
#include <cmath>

Grid::Grid() {
    this->width = this->right_limit - this->left_limit;
    this->height = this->top_limit - this->bottom_limit;

    // Determine the number of horizontal/vertical cells
    if (this->height >= this->width) {
        this->horizontal_cells = static_cast<int>(this->width / this->height * 100); 
        this->vertical_cells = 100;
    } else {
        this->vertical_cells = static_cast<int>(this->height / this->width * 100); 
        this->horizontal_cells = 100;
    }

    // Determine the width and height of every cell
    this->cell_width = this->width / (double) this->horizontal_cells;
    this->cell_height = this->height / (double) this->vertical_cells;

    // Create matrix full of 0
    for (int i = 0; i < horizontal_cells; i++) {
        std::vector<bool> temp_vec;
        for (int j = 0; j < vertical_cells; j++) {
            temp_vec.push_back(false);
        }
        this->matrix.push_back(temp_vec);
    }
}   

void Grid::populate(std::vector<glm::vec2> all_points) {    
    if (all_points.size() > 0) {
        for (auto point : all_points) {
    
            auto coordinates = this->get_grid_coordinates(point.x, point.y);
            this->matrix.at(coordinates[0] - 1).at(coordinates[1] - 1) = true;
        }
    }
}

std::vector<int> Grid::get_grid_coordinates(double x, double y) {
    double x_diff = x - this->left_limit; 
    double y_diff = this->top_limit - y;

    // Determine the coordinates on the grid
    int x_pos = static_cast<int>(floor(x_diff / this->cell_width));
    int y_pos = static_cast<int>(floor(y_diff / this->cell_height));

    std::vector<int> coordinates{x_pos, y_pos};

    return coordinates;
}