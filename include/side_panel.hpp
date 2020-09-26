#ifndef SIDE_PANEL_HPP
#define SIDE_PANEL_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}
#include "font.hpp"
#include <memory>

class SidePanel {
   private:
    GLFWwindow* window;
    std::shared_ptr<Font> font; 
    int id_winner;
    int full_frames = 0;
    int blank_frames = 0;
    int max_frames = 15;
   public:
    SidePanel(GLFWwindow* window, std::shared_ptr<Font> font);
    void draw(int player_count, int max_score);
    void set_winner(int id);
    void update();

};

#endif