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
#include "border_model.hpp"
#include "player_manager.hpp"
#include "font.hpp"
#include "menu.hpp"
#include "side_panel.hpp"
#include "game_over.hpp"

class Game {
   private:
    GLFWwindow* window;
    std::shared_ptr<BorderModel> border_model;
    std::shared_ptr<PlayerManager> player_manager;
    std::shared_ptr<Font> font; 
    std::shared_ptr<Menu> menu;  
    std::shared_ptr<SidePanel> side_panel;
    std::shared_ptr<GameOver> game_over;
    bool has_players;
    int show_win_frames = 0;
   public:
    /**
     * @brief Construct a new Game object
     * 
     * @param window A pointer of the GLFWwindow object
     */
    Game(GLFWwindow* window);

    /**
     * @brief Start the main game loop
     * 
     */
    void loop();

    /**
     * @brief Terminate and clean all the generated objects
     * 
     */
    void terminate();

    /**
     * @brief Get the count of how many players are active
     * 
     * @return int The count of the players
     */
    int get_player_count();
};

void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
