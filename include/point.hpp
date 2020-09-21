#ifndef POINT_HPP
#define POINT_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"
#include "math.h"
}

struct Point {
    GLfloat x;
    GLfloat y;
};

double get_distance(Point p1, Point p2);

#endif