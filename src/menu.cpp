#include "menu.hpp"
#include "user.hpp"

Menu::Menu(GLFWwindow* window) {
    this->window = window;

    auto font = std::make_shared<Font>();
    this->font = font;
}

void Menu::draw() {
    this->font->draw_text("MENU", 0.0f, 0.0f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Menu::update() {
    
}
