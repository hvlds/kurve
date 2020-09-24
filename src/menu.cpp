#include "menu.hpp"
#include "user.hpp"

#include <iostream>

Menu::Menu(GLFWwindow* window) {
    this->window = window;

    auto font = std::make_shared<Font>();
    this->font = font;
}

void Menu::draw() {
    user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(this->window);

    float pos = -100.0f;
    float pos_name_y = 400.0f;

    for (auto player_info : *user_data->player_info) {
        std::string name = player_info.name;
        std::string menu_text = player_info.menu_text;
        int id =player_info.id;
        bool is_active = player_info.is_active;
        glm::vec3 menu_color = player_info.menu_color;
        
        if(is_active == true) {
            this->font->draw_text("READY", 200.0f, pos_name_y + pos, 1.0f, menu_color);
        }

        this->font->draw_text(menu_text, -500.0f, pos_name_y+pos, 1.0f, menu_color);
        pos_name_y -= 100.0f;
    }
    
    this->font->draw_text("Press ENTER to start", -250.0f, 500.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    this->font->draw_text("and press the left key of the player to join the party", -335.0f, 500.0f + pos, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));  
}

void Menu::update() {
    
}
