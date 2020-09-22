#ifndef POINT_HPP
#define POINT_HPP

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"
#include "math.h"
}

class Point {
   public:
    GLfloat x;
    GLfloat y;
    static double get_distance(Point p1, Point p2);
};

class Vector {
   public:
    GLfloat x;
    GLfloat y;
    Vector(Point start, Point end);
    double get_lenght();
    static GLfloat dot_product(Vector v1, Vector v2);
    static GLfloat cross_product(Vector v1, Vector v2);
};


#endif