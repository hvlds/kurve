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

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER
};

class Game {
   private:
    GameState state;
    GLFWwindow* window;
    std::vector<std::shared_ptr<Model>> models;
    std::shared_ptr<PlayerManager> player_manager;
   public:
    Game(GLFWwindow* window);
    void loop();
    void terminate();
};

#endif