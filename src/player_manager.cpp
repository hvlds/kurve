#include "player_manager.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>

PlayerManager::PlayerManager(GLFWwindow* window) {
    srand((unsigned) time(0));
    this->window = window;       
}

void PlayerManager::add_players() {
    std::cout << "---- ADD PLAYERS ----" << std::endl;
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    
    for(auto player_info : *user_data->player_info) {
        if(player_info.is_active == true) {
            auto color = player_info.color;
            auto control = player_info.control;
            int id = player_info.id;
            this->add_player(id, control, color);
        }
    }

    // Calculate the max score for the party
    int players_count = static_cast<int>(this->players.size());
    this->max_score = players_count * 5;
}

void PlayerManager::add_player(
    int id,
    Control control,
    std::array<GLubyte, 3> color) {    
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;

    auto player = std::make_shared<PlayerModel>(id, x, y, color);
    player->set_keys(control);

    this->players.insert({id, player});
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

std::vector<Point> PlayerManager::get_player_trace(int id) {
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

std::vector<Point> PlayerManager::get_oponent_trace(int id) {
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

void PlayerManager::detect_collisions() {
    for (auto item : this->players) {
        int id = item.first;
        auto oponent_points = this->get_oponent_trace(id);
        auto own_points = this->get_player_trace(id);
        std::shared_ptr<PlayerModel> player = item.second;
        Point position = player->get_position();
        for (auto point : oponent_points) {
            double distance = Point::get_distance(point, position);
            if (distance < 0.2) {
                if (player->is_alive == true) {
                    player->is_alive = false;
                    std::cout << "Collision with player!" << std::endl;
                    this->dead_players.push_back(player->get_id());
                }
            }
        }
        if (own_points.size() > 20) {
            for (auto point : own_points) {
                double distance = Point::get_distance(point, position);
                if (distance < 0.3) {
                    if (player->is_alive == true) {
                        player->is_alive = false;
                        std::cout << "Collision with your own line!" << std::endl;
                        this->dead_players.push_back(player->get_id());
                    }
                }
            }
        }
    }
}

std::vector<int> PlayerManager::get_alive_players() {
    std::vector<int> alive_ids;
    for (auto item : this->players) {
        if (item.second->is_alive == true) {
            alive_ids.push_back(item.first);
        }
    }  
    return alive_ids;
}

void PlayerManager::update_score() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    int local_score = 0;
    for (int dead_player_id : this->dead_players) {
        user_data->player_info->at(dead_player_id - 1).score += local_score;
        local_score++; 
    }
    std::vector<int> alive_vec = this->get_alive_players();
    if (alive_vec.size() == 1) {
        int last_id = alive_vec.back();
        user_data->player_info->at(last_id - 1).score += this->players.size();
    }
}

void PlayerManager::check_score() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    std::pair<int, int> best_score;
    for (auto item : this->players) {
        int id = item.first;
        int score = user_data->player_info->at(id - 1).score;
    }
}

void PlayerManager::reset_player(int id) {
    std::shared_ptr<PlayerModel> player = this->players.at(id);

    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;
    
    Point new_position{x, y};

    player->clear();
    player->set_position(new_position);
}

void PlayerManager::reset() {
    // Clear the map with user and vector with dead players
    this->dead_players.clear();

    std::cout << "---- RESET PLAYERS ----" << std::endl;
    for (auto item : this->players) {
        item.second->is_alive = true;
        this->reset_player(item.first);
    }
}

int PlayerManager::get_players_count() {
    return this->players.size();
}

int PlayerManager::get_max_score() {
    return this->max_score;
}

void PlayerManager::terminate() {
    this->players.clear();
}