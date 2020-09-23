#ifndef MENU_HPP
#define MENU_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}
#include "font.hpp"
#include <memory>


class Menu {
   protected:
    GLFWwindow* window;
    std::shared_ptr<Font> font;
   public:
    Menu(GLFWwindow* window);
    void draw();
    void update();

};

#endif