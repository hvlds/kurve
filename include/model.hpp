#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>
#include <memory>

#include "mesh.hpp"
#include "shader.hpp"
#include "gl_calls.hpp"
#include "user.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

enum ModelType {
    MODEL_PLAYER, // Player (yellow circle a.k.a head of the line)
    MODEL_LINE, // Line of the player
    MODEL_BORDER // Border of the game
};

class Model {
   protected:
    user_data_t* user_data;
    virtual void init_uniforms() = 0;
    virtual void init_values() = 0;
   public:
    std::shared_ptr<Mesh> mesh;
    ModelType model_type;
    GLuint shader_id;
    virtual void update(GLFWwindow* window) = 0;
    virtual void draw() = 0;
};

#endif