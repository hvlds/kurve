#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "player_model.hpp"
#include "point.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <map>
#include <memory>
#include <vector>

class PlayerManager {
   protected:
    std::map<int, std::shared_ptr<PlayerModel>> players;
    std::vector<int> dead_players;
    GLFWwindow* window;
   public:
    PlayerManager(GLFWwindow* window);
    void add_player(
        int id,
        Control control,
        std::array<GLubyte, 3> color);
    int get_new_id();
    void update(GLFWwindow* window);
    std::vector<Point> get_all_points();
    std::vector<Point> get_oponent_trace(int id);
    std::vector<Point> get_player_trace(int id);
    Point get_player_position(int id);
    void detect_collisions();
    std::vector<int> get_alive_players();
    void draw();
    void add_players();
};

#endif