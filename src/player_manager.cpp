#include "player_manager.hpp"
#include <stdlib.h>
#include <ctime>
#include <iostream>

PlayerManager::PlayerManager() {
    srand((unsigned) time(0));
}

void PlayerManager::add_player(
    Control control,
    std::array<GLubyte, 3> color) {    
    int random_x= -100 + (rand() % 200);
    int random_y= -100 + (rand() % 200);

    GLfloat x = (GLfloat)random_x / 100;
    GLfloat y = (GLfloat)random_y / 100;

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