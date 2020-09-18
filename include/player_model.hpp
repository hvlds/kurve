#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "model.hpp"
#include "user.hpp"
#include "player_mesh.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include <math.h>
}

#define Y_ANGULAR_VELOCITY 2

class PlayerModel : public Model {
   protected: 
    virtual void init_uniforms() override;
    virtual void init_values() override;
    std::shared_ptr<PlayerMesh> player_mesh;
   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;

    // The Uniform values
    double time;
    GLfloat trans_y;
    GLfloat trans_x;

    PlayerModel();
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
};

#endif