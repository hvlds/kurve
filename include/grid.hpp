#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include <queue>

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
    glm::ivec2 center;
    std::vector<std::vector<bool>> matrix;
    int direction_to_cuadrant(glm::vec2 direction);
    void check_cuadrants(int direction_cuadrant);
    bool in_grid(glm::ivec2 cell);
    int get_distance(glm::ivec2 c1, glm::ivec2 c2);
    std::vector<glm::ivec2> get_neighbours(glm::ivec2 cell);
   public:
    Grid();
    glm::ivec2 get_coordinates(double x, double y);
    void populate(std::vector<glm::vec2> all_points);
    void set_player(glm::vec2 center, glm::vec2 direction);
    glm::ivec2 get_next_cell(glm::ivec2 start, glm::ivec2 goal);
    int get_new_direction(glm::vec2 center, glm::vec2 direction);
    void clear();
    void print();    
};

template<typename T, typename priority_t>
struct PriorityQueue {
  typedef std::pair<priority_t, T> PQElement;
  std::priority_queue<PQElement, std::vector<PQElement>,
                 std::greater<PQElement>> elements;

  inline bool empty() const {
     return elements.empty();
  }

  inline void put(T item, priority_t priority) {
    elements.emplace(priority, item);
  }

  T get() {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
};

#endif