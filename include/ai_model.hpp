#ifndef AI_MODEL_HPP
#define AI_MODEL_HPP

#include "line_model.hpp"
#include "player_mesh.hpp"
#include "player_model.hpp"
#include "point.hpp"
#include "user.hpp"

class AIModel : public PlayerModel {
   private:
    std::vector<glm::vec2> all_points;
    int forward_counter = 0;
   public:
    AIModel(int id, GLfloat x, GLfloat y, glm::vec3 color) : PlayerModel(id, x, y, color) {}
    void update(GLFWwindow* window) override;
    void set_all_points(std::vector<glm::vec2> all_points);
    int plan();
};

#endif