#include "menu.hpp"
#include "user.hpp"

Menu::Menu(GLFWwindow* window) {
    this->window = window;

    auto font = std::make_shared<Font>();
    this->font = font;
}

void Menu::draw() {
    
    this->font->draw_text("press ENTER to start", -250.0f, 500.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    this->font->draw_text("Player 1 <1 Q>", -500.0f, 400.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    this->font->draw_text("Player 2 <L.Ctrl L.Alt>", -500.0f, 300.0f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    this->font->draw_text("Player 3 <M ,>", -500.0f, 200.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    this->font->draw_text("Player 4 <L.Arrow R.Arrow>", -500.0f, 100.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    this->font->draw_text("Player 5 <o p>", -500.0f, 0.0f, 1.0f, glm::vec3(1.0f, 0.11f, 0.68f));
    this->font->draw_text("Player 6 <b n>", -500.0f, -100.0f, 1.0f, glm::vec3(0.7f, 0.7f, 0.7f));

}

void Menu::update() {
    
}
