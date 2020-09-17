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
   private:
    Mesh mesh;
    GLuint shader_id;
    user_data_t* user_data;

   public:
    Model(std::string vs_path, std::string fs_path, user_data_t* user_data);
    void draw();
};

#endif