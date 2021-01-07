#include "user.hpp"
#include <iostream>

void reset_player_info(GLFWwindow* window) {
#ifdef DEBUG
    std::cout << "---- RESET Players ----" << std::endl;
#endif
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    for (auto info : *user_data->player_info) {
        user_data->player_info->at(info.id - 1).is_active = false;
        user_data->player_info->at(info.id - 1).score = 0;
    }
}

std::multimap<int, int, std::greater<int>> get_results(GLFWwindow* window) {
    std::multimap<int, int, std::greater<int>> results;
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    for (auto player : *user_data->player_info) {
        if (player.is_active == true) {
            results.insert(
                std::make_pair(player.score, player.id)
            );
        }
    }
    return results;
}

int get_winner(GLFWwindow* window) {
    auto results = get_results(window);
    int winner = -1;

    for (auto item : results) {
        winner = item.second;
        break;
    }

    return winner;
}