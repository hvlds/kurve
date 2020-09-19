#ifndef LINE_MODEL_HPP
#define LINE_MODEL_HPP

#include "model.hpp"
#include "user.hpp"
#include "line_mesh.hpp"
#include "point.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include <math.h>
}

#include <vector>

class LineModel : public Model {
   protected: 
    virtual void init_uniforms() override;
    virtual void init_values() override;
    std::vector<Point> points;    
   public:
    LineModel();
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
};

#endif