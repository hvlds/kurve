#include "game_over.hpp"
#include "user.hpp"

GameOver::GameOver(GLFWwindow* window, std::shared_ptr<Font> font) {
    this->window = window;
    this->font = font;
}

void GameOver::draw() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

    this->font->draw_text(
        "GAME OVER",
        -150.0f,
        450.0f,
        1.5f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    
    float pos = -200.0f;
    float pos_name_y = 350.0f;
    float ready_extra_margin = 150.0f;
    int active_counter = 0;
    
    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        std::string menu_text = player_info.menu_text;
        int id = player_info.id;
        bool is_active = player_info.is_active;
        glm::vec3 menu_color = player_info.menu_color;
        int score = player_info.score;

        if (is_active == true) {
            this->font->draw_text(
                std::to_string(score),
                200.0f + ready_extra_margin,
                pos_name_y + pos,
                0.75f,
                menu_color);
            active_counter++;
            this->font->draw_text(
                name, 
                -500.0f, 
                pos_name_y + pos, 
                0.75f, 
                menu_color);
            pos_name_y -= 75.0f;
        }
    }

    this->font->draw_text(
        "Press SPACE to start a new party",
        -300.0f,
        -225.0f + pos,
        0.7f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    
}

void GameOver::update() {

}