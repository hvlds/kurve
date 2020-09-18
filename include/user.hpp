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
    
} user_data_t;

#endif