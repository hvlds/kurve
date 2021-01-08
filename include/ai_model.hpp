#ifndef AI_MODEL_HPP
#define AI_MODEL_HPP

#include "line_model.hpp"
#include "player_mesh.hpp"
#include "player_model.hpp"
#include "point.hpp"
#include "user.hpp"

class AIModel : public PlayerModel {
   public:
    AIModel(int id, GLfloat x, GLfloat y, glm::vec3 color) : PlayerModel(id, x, y, color) {}
    void update(GLFWwindow* window) override;
};

#endif