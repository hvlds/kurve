#ifndef AI_MODEL_HPP
#define AI_MODEL_HPP

#include "line_model.hpp"
#include "player_mesh.hpp"
#include "player_model.hpp"
#include "point.hpp"
#include "user.hpp"
#include "grid.hpp"

#include <memory>

class AIModel : public PlayerModel {
   private:
    std::vector<glm::vec2> all_points;
    int forward_counter = 0;
    glm::vec2 goal;
    std::shared_ptr<Grid> grid;
    int counter_plan = 0;
    int last_direction = 0;
    GLFWwindow* window;
   public:
    AIModel(GLFWwindow* window, int id, GLfloat x, GLfloat y, glm::vec3 color) : PlayerModel(window, id, x, y, color) {
        this->window = window;
        auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
        this->grid = user_data->grid;
    }
    void update(GLFWwindow* window) override;
    void set_all_points(std::vector<glm::vec2> all_points);
    void plan();
    void clear() override;
};

#endif