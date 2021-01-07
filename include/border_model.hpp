#ifndef BORDER_MODEL_HPP
#define BORDER_MODEL_HPP

#include "model.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include <math.h>
}

class BorderModel: public Model {
   protected:
    virtual void init_uniforms() override;
    virtual void init_values() override;
   public:
    BorderModel();
    virtual void update(GLFWwindow* window) override;
    virtual void draw(GLFWwindow* window) override;
};

#endif