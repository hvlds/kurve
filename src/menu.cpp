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

    if (user_data->is_player_1_active == true) {
        this->font->draw_text("READY", 200.0f, 400.0 + pos, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if (user_data->is_player_2_active == true) {
        this->font->draw_text("READY", 200.0f, 300.0f+ pos, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (user_data->is_player_3_active == true) {
        this->font->draw_text("READY", 200.0f, 200.0f+ pos, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
    if (user_data->is_player_4_active == true) {
        this->font->draw_text("READY", 200.0f, 100.0f+ pos, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    if (user_data->is_player_5_active == true) {
        this->font->draw_text("READY", 200.0f, 0.0f+ pos, 1.0f, glm::vec3(1.0f, 0.11f, 0.68f));
    }
    if (user_data->is_player_6_active == true) {
        this->font->draw_text("READY", 200.0f, -100.0f+ pos, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f));
    }
    
    this->font->draw_text("Press ENTER to start", -250.0f, 500.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    this->font->draw_text("and press the left key of the player to join the party", -335.0f, 500.0f + pos, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));    
    this->font->draw_text("Gryffindor <L.Ctrl L.Alt>", -500.0f, 400.0f+pos, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    this->font->draw_text("Slytherin  <1 Q>", -500.0f, 300.0f+ pos, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    this->font->draw_text("Hufflepuff <M ,>", -500.0f, 200.0f+ pos, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    this->font->draw_text("Ravenclaw  <L.Arrow R.Arrow>", -500.0f, 100.0f+ pos, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    this->font->draw_text("Muggle     <O P>", -500.0f, 0.0f+ pos, 1.0f, glm::vec3(1.0f, 0.11f, 0.68f));
    this->font->draw_text("Squib      <B N>", -500.0f, -100.0f+ pos, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f));

}

void Menu::update() {
    
}
