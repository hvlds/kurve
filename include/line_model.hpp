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
#include <memory>

class LineModel : public Model {
   protected: 
    virtual void init_uniforms() override;
    virtual void init_values() override;
    std::vector<Point> points;
    std::shared_ptr<LineMesh> line_mesh;
    std::array<GLubyte, 3> color;
   public:
    // The uniforms locations
    GLint start_pos_x_loc;
    GLint start_pos_y_loc;

    // The uniforms values
    GLfloat start_pos_y;
    GLfloat start_pos_x;

    LineModel(Point point, std::array<GLubyte, 3> color);
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
    void add_point(Point point);
    void set_color(std::array<GLubyte, 3> color);
    std::vector<Point> get_points();
    void set_start_position(Point point);
    void clear();
};

#endif