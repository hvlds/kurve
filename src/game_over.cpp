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
        glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_medium");
    
    float pos = -200.0f;
    float pos_name_y = 350.0f;
    float ready_extra_margin = 150.0f;
    int active_counter = 0;

    auto results = get_results(this->window);
    int id_winner = get_winner(this->window);
    auto players = *user_data->player_info;
    
    for (auto item : results) {
        std::string name = players.at(item.second - 1).name;
        std::string menu_text = players.at(item.second - 1).menu_text;
        int id = players.at(item.second - 1).id;
        bool is_active = players.at(item.second - 1).is_active;
        glm::vec3 color = players.at(item.second - 1).color;
        int score = players.at(item.second - 1).score;

        if (is_active == true) {
            if (id == id_winner) {
                if (this->blank_frames <= this->max_frames) {
                    this->font->draw_text(
                        std::to_string(score),
                        200.0f + ready_extra_margin,
                        pos_name_y + pos,
                        0.75f,
                        color, true, "regular_medium");
                    this->font->draw_text(
                        name,
                        -500.0f,
                        pos_name_y + pos,
                        0.75f,
                        color, true, "regular_medium");
                    this->blank_frames++;
                } else if (this->full_frames <= this->max_frames) {
                    this->full_frames++;
                } else {
                    this->blank_frames = 0;
                    this->full_frames = 0;
                }

            } else {
                this->font->draw_text(
                    std::to_string(score), 200.0f + ready_extra_margin,
                    pos_name_y + pos, 0.75f, color, false, "regular_medium");
                this->font->draw_text(
                    name, -500.0f, pos_name_y + pos, 0.75f, color, false, "regular_medium");
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
        glm::vec3(1.0f, 1.0f, 1.0f),
        false, "regular_medium");
    
}

void GameOver::update() {

}