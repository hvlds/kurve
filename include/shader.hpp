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
    /**
     * @brief Construct a new Shader object
     * 
     * @param vs_path Path of the vertex shader
     * @param fs_path Path of the fragment shader
     * @param shader_id Pointer of the variable to store the id of the new 
     * compiled shader
     */
    Shader(std::string vs_path, std::string fs_path, GLuint* shader_id);
    /**
     * @brief Compile a shader to be use in the OpenGL pipeline
     * 
     * @param type Type of the shader
     * @param shader_path Path of the shader
     * @param shader_tag Internal tag to be used with the shader
     * @return GLuint The ID of the compiled shader
     */
    static GLuint compile(GLenum type, const char* shader_path, const char* shader_tag);
};

#endif