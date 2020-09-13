#ifndef USER_HPP
#define USER_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
}
#include <vector>

#include "gl_calls.hpp"

typedef struct {
    // Dimensions of the window:
    int window_width;
    int window_height;

    // The current shader program:
    GLuint shader_program;

    // The uniform locations:
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLint trans_vec_x_loc;
    GLint trans_vec_y_loc;
    GLint trans_loc;

    std::vector<gl_obj_t> vec_obj;

    // The texture object:
    GLuint tex;

    // The number of vertices to render:
    int vertex_data_count;

    // The model:
    double time;
    GLfloat trans_y;
    GLfloat trans_x;
    GLuint shading_case;
    int count = 0;

} user_data_t;

#endif