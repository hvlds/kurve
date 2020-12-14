#include "game_over.hpp"
#include "user.hpp"
#include <iostream>

GameOver::GameOver(GLFWwindow* window, std::shared_ptr<Font> font) {
    this->window = window;
    this->font = font;
}

void GameOver::draw() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

    this->font->draw_text(
        "GAME OVER",
        -160.0f,
        450.0f,
        1.5f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    
    float pos = -200.0f;
    float pos_name_y = 350.0f;
    float ready_extra_margin = 150.0f;
    int active_counter = 0;
    int id_winner = this->get_winner();
    
    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        std::string menu_text = player_info.menu_text;
        int id = player_info.id;
        bool is_active = player_info.is_active;
        glm::vec3 menu_color = player_info.menu_color;
        int score = player_info.score;

        if (is_active == true) {
            if (id == id_winner) {
                if (this->blank_frames <= this->max_frames) {
                    this->font->draw_text(
                        std::to_string(score),
                        200.0f + ready_extra_margin,
                        pos_name_y + pos,
                        0.75f,
                        menu_color);
                    this->font->draw_text(
                        name,
                        -500.0f,
                        pos_name_y + pos,
                        0.75f,
                        menu_color);
                    this->blank_frames++;
                } else if (this->full_frames <= this->max_frames) {
                    this->full_frames++;
                } else {
                    this->blank_frames = 0;
                    this->full_frames = 0;
                }

            } else {
                this->font->draw_text(
                    std::to_string(score),
                    200.0f + ready_extra_margin,
                    pos_name_y + pos,
                    0.75f,
                    menu_color);
                this->font->draw_text(
                    name, 
                    -500.0f, 
                    pos_name_y + pos, 
                    0.75f, 
                    menu_color);
            }

            pos_name_y -= 75.0f;
            active_counter++;
        }
    }

    this->font->draw_text(
        "Press SPACE to start a new party",
        -250.0f,
        -225.0f + pos,
        0.7f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    
}

int GameOver::get_winner() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    std::pair<int, int> best_player;
    bool is_first = true;
    
    for (auto player : *user_data->player_info) {
        if (player.is_active == true) {
            int score = player.score;
            int id = player.id;
            auto temp_pair = std::make_pair(id, score);
            if (is_first == true) {
                best_player = temp_pair;
                is_first = false;
            } else {
                if (score > best_player.second) {
                    best_player = temp_pair;
                }
            }
        }
    }

    return best_player.first;
}

void GameOver::update() {

}