#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "player_model.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <map>
#include <memory>

class PlayerManager {
   protected:
    std::map<int, std::shared_ptr<PlayerModel>> players;
   public:
    PlayerManager();
    void add_player(
        Control control,
        std::array<GLubyte, 3> color);
    int get_new_id();
    void update(GLFWwindow* window);
    void draw();
};

#endif