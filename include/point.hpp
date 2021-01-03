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

double get_length(glm::vec2 v);
double dot_product(glm::vec2 v1, glm::vec2 v2);
double get_angle(glm::vec2 v1, glm::vec2 v2);
double cross_product(glm::vec2 v1, glm::vec2 v2);


#endif