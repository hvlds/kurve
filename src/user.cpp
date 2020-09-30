#include "user.hpp"

void reset_player_info(GLFWwindow* window) {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    for (auto info : *user_data->player_info) {
        user_data->player_info->at(info.id - 1).is_active = false;
        user_data->player_info->at(info.id - 1).score = 0;
    }
}