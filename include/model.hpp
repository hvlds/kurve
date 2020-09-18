#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>

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

class Model {
   protected:
    Mesh mesh;
    GLuint shader_id;
    user_data_t* user_data;

   public:
    virtual void update() = 0;
    virtual void init_uniforms() = 0;
    void draw() {
        this->mesh.draw();
    }
};

#endif