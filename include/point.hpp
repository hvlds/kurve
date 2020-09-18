#ifndef POINT_HPP
#define POINT_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"
}

struct Point {
    GLfloat x;
    GLfloat y;
};

#endif