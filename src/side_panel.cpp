#include "side_panel.hpp"
#include "user.hpp"

SidePanel::SidePanel(GLFWwindow* window, std::shared_ptr<Font> font) {
    this->window = window;
    this->font = font;
    this->id_winner = -1;
}

void SidePanel::draw(int player_count, int max_score) {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    GameState game_state = user_data->game_state;

    // Show the right player names
    float y_pos = 400.0f;
    float count = 0;

    // Draw the score at the top of the Side Panel
    this->font->draw_text(
        std::to_string(max_score), 460.0f, 500.0f, 1.2f, glm::vec3(1.0f, 1.0f, 1.0f), false, "bold_medium");
    this->font->draw_text(
        "Goal", 455.0f, 450.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_medium");
    this->font->draw_text(
        "2 points diff", 415.0f, 430.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");

    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        auto color = player_info.color;
        if (player_info.is_active == true) {
            y_pos = y_pos - 50.0f;
            auto score_text = std::to_string(player_info.score);
            if (game_state == GAME_WIN) {
                if (player_info.id == this->id_winner) {
                    this->font->draw_text(
                        name, 450.0f, y_pos, 1.0f, color, true, "regular_small");
                    this->font->draw_text(
                        score_text, 400.0f, y_pos, 1.0f, color, true, "regular_small"); 
                } else {
                    this->font->draw_text(
                        name, 450.0f, y_pos, 1.0f, color, false, "regular_small");
                    this->font->draw_text(
                        score_text, 400.0f, y_pos, 1.0f, color, false, "regular_small");
                }
            } else {
                this->font->draw_text(
                    name, 450.0f, y_pos, 1.0f, color, false, "regular_small");
                this->font->draw_text(
                    score_text, 400.0f, y_pos, 1.0f, color, false, "regular_small");
            }
            count += 1.0f;
        }
    }

    std::string fps_str = std::to_string(user_data->fps);
    this->font->draw_text(
            fps_str, 400.0f, -250.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");

    if (game_state == GAME_PAUSE) {
        this->font->draw_text(
            "Press SPACE", 400.0f, -300.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
        this->font->draw_text(
            "to continue", 400.0f, -325.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
    } else if (game_state == GAME_ACTIVE) {
        this->font->draw_text(
            "Press SPACE", 400.0f, -300.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
        this->font->draw_text(
            "to pause", 400.0f, -325.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
    } else if (game_state == GAME_TRANSITION) {
        this->font->draw_text(
            "Press SPACE", 400.0f, -300.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
        this->font->draw_text(
            "to start", 400.0f, -325.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
    }
    
    this->font->draw_text(
            "Press ESCAPE", 400.0f, -375.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f),false, "regular_small");
        this->font->draw_text(
            "to restart", 400.0f, -400.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), false, "regular_small");
}

void SidePanel::update() {

}

void SidePanel::set_winner(int id) {
    this->id_winner = id;
    
}