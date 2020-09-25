#ifndef SHADER_HPP
#define SHADER_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}

#include <string>

#include "user.hpp"

class Shader {
   private:
    /**
     * @brief Read a GLSL shader file. The extension doesn't need a particular
     * format.
     * 
     * @param path Path of the shader file
     * @return char* Output buffer of the file
     */
    static char* read_from_file(const char* path);

   public:
    Shader(std::string vs_path, std::string fs_path, GLuint* shader_id);
    static GLuint compile(GLenum type, const char* shader_path, const char* shader_tag);
    GLuint compile();
};

#endif