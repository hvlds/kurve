#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "model.hpp"
#include "user.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

class PlayerModel : public Model {
   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;

    // The Uniform values
    double time;
    GLfloat trans_y;
    GLfloat trans_x;

    PlayerModel(user_data_t* user_data);
    virtual void update() override;
    virtual void init_uniforms() override;
};

#endif