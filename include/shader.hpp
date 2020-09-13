#ifndef SHADER_HPP
#define SHADER_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

class Shader {
   private:
    int id;
    static char* read_from_file(const char* path);
    static GLuint compile(GLenum type, const char* shader_path, const char* shader_tag);

   public:
    Shader(const char* path);
    GLuint compile();
};

#endif