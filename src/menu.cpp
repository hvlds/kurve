#include "menu.hpp"

#include <iostream>

#include "user.hpp"

Menu::Menu(GLFWwindow* window) {
    this->window = window;

    auto font = std::make_shared<Font>();
    this->font = font;
}

void Menu::draw() {
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

    float pos = -100.0f;
    float pos_name_y = 350.0f;
    float ready_extra_margin = 150.0f;
    int active_counter = 0;

    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        std::string menu_text = player_info.menu_text;
        int id = player_info.id;
        bool is_active = player_info.is_active;
        glm::vec3 menu_color = player_info.menu_color;

        if (is_active == true) {
            this->font->draw_text(
                "READY",
                200.0f + ready_extra_margin,
                pos_name_y + pos,
                0.9f,
                menu_color);
            active_counter++;
        }

        this->font->draw_text(
            menu_text, 
            -500.0f, 
            pos_name_y + pos, 
            0.9f, 
            menu_color);
        pos_name_y -= 100.0f;
    }

    this->font->draw_text(
        "Press the left key of the player to join the party",
        -400.0f,
        -275.0f + pos,
        0.7f,
        glm::vec3(1.0f, 1.0f, 1.0f));
    this->font->draw_text(
        "Achtung, die Kurve!",
        -300.0f,
        550.0f + pos,
        1.5f,
        glm::vec3(1.0f, 1.0f, 1.0f));

    if (active_counter >= 2) {
        this->font->draw_text(
            "Press ENTER to start",
            -150.0f,
            -350.0f + pos,
            0.7f,
            glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void Menu::update() {
}
