#ifndef GAME_OVER_HPP
#define GAME_OVER_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}
#include "font.hpp"
#include <memory>

class GameOver {
   private:
    GLFWwindow* window;
    std::shared_ptr<Font> font;
   public:
    GameOver(GLFWwindow* window, std::shared_ptr<Font> font);
    void draw();
    void update();
};

#endif