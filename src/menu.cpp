#include "menu.hpp"

#include <iostream>

#include "user.hpp"

Menu::Menu(GLFWwindow* window, std::shared_ptr<Font> font) {
    this->window = window;
    this->font = font;
}

void Menu::draw() {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

    float pos = -200.0f;
    float pos_name_y = 350.0f;
    float ready_extra_margin = 150.0f;
    int active_counter = 0;

    this->font->draw_text(
        "             <Left Right>",
        -500.0f,
        pos_name_y + pos + 75,
        0.75f,
        glm::vec3(1.0f, 1.0f, 1.0f));

    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        std::string menu_text = player_info.menu_text;
        // int id = player_info.id;
        bool is_active = player_info.is_active;
        glm::vec3 color = player_info.color;

        if (is_active == true) {
            this->font->draw_text(
                "READY",
                200.0f + ready_extra_margin,
                pos_name_y + pos,
                0.75f,
                color);
            active_counter++;
        }

        this->font->draw_text(
            menu_text, 
            -500.0f, 
            pos_name_y + pos, 
            0.75f, 
            color);
        pos_name_y -= 75.0f;
    }

    this->font->draw_text(
        "Press the left key of the player to join the party",
        -420.0f,
        -225.0f + pos,
        0.7f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    this->font->draw_text(
        "Achtung, die Kurve!",
        -330.0f,
        450.0f,
        1.5f,
        glm::vec3(1.0f, 1.0f, 1.0f));

    if (active_counter >= 2) {
        this->font->draw_text(
            "Press SPACE to start",
            -180.0f,
            -300.0f + pos,
            0.7f,
            glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void Menu::update() {
}
