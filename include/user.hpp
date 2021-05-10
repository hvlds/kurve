#ifndef USER_HPP
#define USER_HPP

#include "gl_calls.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <vector>
#include <array>
#include <memory>

#include "grid.hpp"

/**
 * @brief A user can move left and right. The GLFW keys
 * are stored in Control.
 * 
 */
struct Control {
    int left_key;
    int right_key;
};

/**
 * @brief States of the game.
 * 
 */
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER,
    GAME_PAUSE,
    GAME_TRANSITION,
    GAME_ESCAPE,
    GAME_EXIT
};

/**
 * @brief General information for every player.
 * 
 */
typedef struct {
    bool is_active;
    int id;
    std::string name;
    std::string menu_text;
    Control control;
    glm::vec3 color;
    int score;
    bool is_AI;
} player_info_t;

/**
 * @brief Information of the current user.
 * 
 */
typedef struct {
    // Dimensions of the window:
    int window_width;
    int window_height;
    bool is_fullscreen;
    GameState game_state;
    double delta_time;
    bool has_AI;
    int fps;
    std::vector<player_info_t>* player_info;
    std::shared_ptr<Grid> grid;
} user_data_t;

void reset_player_info(GLFWwindow* window);
int get_winner(GLFWwindow* window);
std::multimap<int, int, std::greater<int>> get_results(GLFWwindow* window);

#endif