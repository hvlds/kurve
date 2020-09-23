#include "player_manager.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>

PlayerManager::PlayerManager(GLFWwindow* window) {
    srand((unsigned) time(0));
    this->window = window;       
}

void PlayerManager::add_players() {
    user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);
    std::cout << user_data->is_player_1_active << std::endl;

    if (user_data->is_player_1_active == true) {
        Control control_1 = {
            GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT
        };
        std::array<GLubyte, 3> color_1 = {0xFF, 0x00, 0x00};
        this->add_player(control_1, color_1);
    }

    if (user_data->is_player_2_active == true) {
        Control control_2 = {
            GLFW_KEY_1, GLFW_KEY_Q
        };
        std::array<GLubyte, 3> color_2 = {0x00, 0xFF, 0x00};
        this->add_player(control_2, color_2);
    }
    
    if (user_data->is_player_3_active == true) {
        Control control_3 = {
            GLFW_KEY_M, GLFW_KEY_COMMA
        };
        std::array<GLubyte, 3> color_3 = {0xFF, 0xFF, 0x00};
        this->add_player(control_3, color_3);
    }

    if (user_data->is_player_4_active == true) {
        Control control_4 = {
            GLFW_KEY_LEFT, GLFW_KEY_RIGHT 
        };
        std::array<GLubyte, 3> color_4 = {0x00, 0x00, 0xFF};
        this->add_player(control_4, color_4);
    }

    if (user_data->is_player_5_active == true) {
        Control control_5 = {
            GLFW_KEY_O, GLFW_KEY_P
        };
        std::array<GLubyte, 3> color_5 = {0xDD, 0x00, 0xDD};
        this->add_player(control_5, color_5);
    }

    if (user_data->is_player_6_active == true) {
        Control control_6 = {
            GLFW_KEY_B, GLFW_KEY_N
        };
        std::array<GLubyte, 3> color_6 = {0xDD, 0xDD, 0xDD};
        this->add_player(control_6, color_6);
    } 

}

void PlayerManager::add_player(
    Control control,
    std::array<GLubyte, 3> color) {    
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;

    int new_id = this->get_new_id();
    auto player = std::make_shared<PlayerModel>(new_id, x, y, color);
    player->set_keys(control);

    this->players.insert({new_id, player});
}

int PlayerManager::get_new_id() {
    int new_id = 0;
    if (!this->players.empty()) {
        for (auto item : this->players) {
            if (item.first > new_id) {
                new_id = item.first;
            }
        }
    }
    new_id++;
    return new_id;
}

void PlayerManager::update(GLFWwindow* window) {
    for (auto item : this->players) {
        item.second->update(window);
    }
}

void PlayerManager::draw() {
    for (auto item : this->players) {
        item.second->draw();
    }
}

std::vector<Point> PlayerManager::get_all_points() {
    std::vector<Point> all_points{};
    for (auto item : this->players) {
        auto temp_points = item.second->get_line_points();
        for (auto point : temp_points) {
            all_points.push_back(point);
        }
    }
    return all_points;
}

std::vector<Point> PlayerManager::get_player_points(int id) {
    std::shared_ptr<PlayerModel> player = this->players.at(id);
    std::vector<Point> all_points = player->get_line_points();
    std::size_t point_count = all_points.size();
    int limit = 20;
    if (point_count >= limit) {
        for (int i = 0; i < limit; i++) {
            all_points.pop_back();
        }
    }
    return all_points;
}

std::vector<Point> PlayerManager::get_oponent_points(int id) {
    std::vector<Point> oponent_points{};
    for (auto item : this->players) {
        if (item.first != id) {
            auto temp_points = item.second->get_line_points();
            for (auto point : temp_points) {
                oponent_points.push_back(point);
            }
        }
    }
    return oponent_points;
}

Point PlayerManager::get_player_position(int id) {
    return this->players.at(id)->get_position();
}

std::vector<int> PlayerManager::get_ids() {
    std::vector<int> keys;
    for (auto item : this->players) {
        keys.push_back(item.first);
    }
    return keys;
}

void PlayerManager::detect_collisions() {
    for (auto item : this->players) {
        int id = item.first;
        auto oponent_points = this->get_oponent_points(id);
        auto own_points = this->get_player_points(id);
        std::shared_ptr<PlayerModel> player = item.second;
        Point position = player->get_position();
        for (auto point : oponent_points) {
            double distance = Point::get_distance(point, position);
            if (distance < 0.5) {
                player->is_active = false;
                std::cout << "Collision with player!" << std::endl;
            }
        }
        if (own_points.size() > 20) {
            for (auto point : own_points) {
                double distance = Point::get_distance(point, position);
                if (distance < 0.5) {
                    player->is_active = false;
                    std::cout << "Collision with your own line!" << std::endl;
                }
            }
        }
    }
}

std::vector<int> PlayerManager::get_active_players() {
    std::vector<int> active_ids;
    for (auto item : this->players) {
        if (item.second->is_active == true) {
            active_ids.push_back(item.first);
        }
    }  
    return active_ids;
}
