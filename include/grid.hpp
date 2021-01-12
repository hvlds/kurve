#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Grid {
   private:
    double left_limit = -18.75;
    double right_limit = 13.5;
    double top_limit = 18.75;
    double bottom_limit = -18.75;
    double width;
    double height;
    int horizontal_cells;
    int vertical_cells;
    double cell_width;
    double cell_height;
    std::pair<int, int> center;
    std::vector<std::vector<bool>> matrix;
    int direction_to_cuadrant(glm::vec2 direction);
    std::pair<int, int> get_coordinates(double x, double y);
    void check_cuadrants(int direction_cuadrant);
    bool in_grid(std::pair<int, int> cell);
    int get_distance(std::pair<int, int> c1, std::pair<int, int> c2);
    std::vector<std::pair<int, int>> get_neighbours(std::pair<int, int> cell);
   public:
    Grid();
    void populate(std::vector<glm::vec2> all_points);
    void set_player(glm::vec2 center, glm::vec2 direction);
    void clear();    
};

#endif