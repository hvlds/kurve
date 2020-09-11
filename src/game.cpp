#include "game.hpp"

Game::Game(GLFWwindow* window) {
    this->window = window;
}

int Game::loop() {
    std::cout << "Starting loop!" << std::endl;
    return 0;
}