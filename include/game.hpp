#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>

extern "C" {
	// Include the GLAD loader *before* including GLFW!
	#include "glad/glad.h"
	
	// Include the GLFW library (should be the same for all OS):
	#include <GLFW/glfw3.h>
}

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER
};

class Game {
   public:
    GameState state;
    GLFWwindow* window;
    Game(GLFWwindow* window);
    int loop();
};

#endif