#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <memory>

extern "C" {
	// Include the GLAD loader *before* including GLFW!
	#include "glad/glad.h"
	
	// Include the GLFW library (should be the same for all OS):
	#include <GLFW/glfw3.h>
}
#include "model.hpp"
#include "player_model.hpp"
#include "player_manager.hpp"
#include "font.hpp"
#include "menu.hpp"

class Game {
   private:
    GLFWwindow* window;
    std::vector<std::shared_ptr<Model>> models;
    std::shared_ptr<PlayerManager> player_manager;
    std::shared_ptr<Font> font; 
    std::shared_ptr<Menu> menu;  
    int count; 
   public:
    Game(GLFWwindow* window);
    int generate_fonts();
    void loop();
    void terminate();
    int get_player_count();
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


#endif
