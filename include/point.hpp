#ifndef POINT_HPP
#define POINT_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"
#include "math.h"
}

double get_distance(glm::vec2 p1, glm::vec2 p2);

class Vector {
   public:
    GLfloat x;
    GLfloat y;
    Vector(glm::vec2 start, glm::vec2 end);
    Vector(GLfloat x, GLfloat y);
    double get_length();
    static double dot_product(Vector v1, Vector v2);
    static double cross_product(Vector v1, Vector v2);
    static double angle(Vector v1, Vector v2);
};


#endif