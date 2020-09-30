#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "player_model.hpp"
#include "user.hpp"
#include "point.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <map>
#include <memory>
#include <vector>

class PlayerManager {
   protected:
    std::map<int, std::shared_ptr<PlayerModel>> players;
    std::vector<int> dead_players;
    GLFWwindow* window;
    void reset_player(int id);
    std::vector<Point> get_oponent_trace(int id);
    std::vector<Point> get_player_trace(int id);
    Point get_player_position(int id);
    int max_score;
   public:
    bool is_updated = false;
    PlayerManager(GLFWwindow* window);
    void add_player(
        int id,
        Control control,
        std::array<GLubyte, 3> color);
    void update(GLFWwindow* window);
    std::vector<Point> get_all_points();

    /**
     * @brief Detect all the collisions between players and lines.
     * Players with players and players with lines.
     * 
     */
    void detect_collisions();

    /**
     * @brief Get a vector with the IDs of all the players that are
     * still alive
     * 
     * @return std::vector<int> A vector with the IDs
     */
    std::vector<int> get_alive_players();

    std::vector<int> get_dead_players();

    /**
     * @brief Get how many players are actually playing (max. 6)
     * 
     * @return int 
     */
    int get_players_count();

    /**
     * @brief Add all the players that were confirmed in the Menu
     * 
     */
    void add_players();

    /**
     * @brief Update the scores for every player
     * 
     */
    void update_score();

    /**
     * @brief Check the score of every player and check if some
     * 
     */
    GameState check_score();
    int get_max_score();
    void draw();
    void reset();
    void terminate();
};

#endif