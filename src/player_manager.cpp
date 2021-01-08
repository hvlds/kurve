#include "player_manager.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>

PlayerManager::PlayerManager(GLFWwindow* window) {
    srand((unsigned) time(0));
    this->window = window; 
}

void PlayerManager::add_players() {
#ifdef DEBUG
    std::cout << "---- ADD PLAYERS ----" << std::endl;
#endif
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    
    for(auto player_info : *user_data->player_info) {
        if(player_info.is_active == true) {
            auto color = player_info.color;
            auto control = player_info.control;
            int id = player_info.id;
            this->add_player(id, control, color, player_info.is_AI);
        }
    }

    // Calculate the max score for the party
    int players_count = static_cast<int>(this->players.size());
    this->max_score = players_count * 5;
}

void PlayerManager::add_player(
    int id,
    Control control,
    glm::vec3 color,
    bool is_AI) {   
    // Generate a random start position for the new player 
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;

    if (is_AI == false) {
        auto player = std::make_shared<PlayerModel>(id, x, y, color);
        player->set_keys(control);
        this->players.insert({id, player});
    } else {
        auto AI = std::make_shared<AIModel>(id, x, y, color);
        this->players.insert({id, AI});
    }

}

void PlayerManager::update(GLFWwindow* window) {
    for (auto item : this->players) {
        item.second->update(window);
    }
}

void PlayerManager::draw() {
    for (auto item : this->players) {
        item.second->draw(this->window);
    }
}

std::vector<glm::vec2> PlayerManager::get_all_points() {
    std::vector<glm::vec2> all_points{};
    for (auto item : this->players) {
        auto temp_points = item.second->get_line_points();
        for (auto point : temp_points) {
            all_points.push_back(point);
        }
    }
    return all_points;
}

std::vector<glm::vec2> PlayerManager::get_player_trace(int id) {
    std::shared_ptr<PlayerModel> player = this->players.at(id);
    std::vector<glm::vec2> all_points = player->get_line_points();
    std::size_t point_count = all_points.size();
    std::size_t limit = 20;
    if (point_count >= limit) {
        for (std::size_t i = 0; i < limit; i++) {
            all_points.pop_back();
        }
    }
    return all_points;
}

std::vector<glm::vec2> PlayerManager::get_oponent_trace(int id) {
    std::vector<glm::vec2> oponent_points{};
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

glm::vec2 PlayerManager::get_player_position(int id) {
    return this->players.at(id)->get_position();
}

void PlayerManager::detect_collisions() {
    for (auto item : this->players) {
        int id = item.first;
        auto oponent_points = this->get_oponent_trace(id);
        auto own_points = this->get_player_trace(id);
        std::shared_ptr<PlayerModel> player = item.second;
        glm::vec2 position = player->get_position();

        // Check if player is alive (collision with border)
        bool in_dead_players = false;
        for (int id_dead : this->dead_players) {
            if (id_dead == id) {
                in_dead_players = true;
                break;
            }
        }

        if (player->is_alive == false && in_dead_players == false) {
            this->dead_players.push_back(player->get_id());
        } else {
            // Collisions with other players
            for (auto point : oponent_points) {
                auto distance = glm::length(position - point);
                if (distance < 0.3) {
                    if (player->is_alive == true) {
                        player->is_alive = false;
#ifdef DEBUG
                        std::cout << "Collision with player!" << std::endl;
#endif
                        this->dead_players.push_back(player->get_id());
                        break;
                    }
                }
            }

            // Collisions with all the lines (include of the base player)
            if (own_points.size() > 20 && player->is_alive == true) {
                for (auto point : own_points) {
                    auto distance = glm::length(position - point);
                    if (distance < 0.3) {
                        if (player->is_alive == true) {
                            player->is_alive = false;
#ifdef DEBUG
                            std::cout << "Collision with your own line!" 
                                << std::endl;
#endif
                            this->dead_players.push_back(player->get_id());
                            break;
                        }
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

std::vector<int> PlayerManager::get_dead_players() {
    return this->dead_players;
}

void PlayerManager::update_score() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    if (this->is_updated == false) {
        int local_score = 0;
        for (int dead_player_id : this->dead_players) {
            user_data->player_info->at(dead_player_id - 1).score += local_score;
            local_score++; 
        }
        
        std::vector<int> alive_vec = this->get_alive_players();
        if (alive_vec.size() == 1) {
            int last_id = alive_vec.back();
            user_data->player_info->at(last_id - 1).score += 
                this->players.size() - 1;
        }

        this->is_updated = true;
    }
}

GameState PlayerManager::check_score() {
    GameState new_state = GAME_ACTIVE;

    auto results = get_results(this->window);
    int first_place, second_place;
    int count = 0;
    for (auto item : results) {
        if (count == 0) {
            first_place = item.first;
            count++;
        } else if (count == 1) {
            second_place = item.first;
            break;
        }
    }
    
    if (first_place >= second_place + 2 && first_place >= this->max_score) {
        new_state = GAME_OVER;
    }

    return new_state;
}

void PlayerManager::reset_player(int id) {
    std::shared_ptr<PlayerModel> player = this->players.at(id);

    // Generate a new random position for the player
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat) random_x / 10;
    GLfloat y = (GLfloat) random_y / 10;
    
    glm::vec2 new_position{x, y};

    player->clear();
    player->set_position(new_position);
}

void PlayerManager::reset() {
    // Clear the map with user and vector with dead players
    this->dead_players.clear();
#ifdef DEBUG
    std::cout << "---- RESET PLAYERS ----" << std::endl;
#endif
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
    this->dead_players.clear();
}