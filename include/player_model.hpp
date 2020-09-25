#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "line_model.hpp"
#include "model.hpp"
#include "player_mesh.hpp"
#include "point.hpp"
#include "user.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>
#include <math.h>
}

#include <array>
#include <memory>
#include <vector>

#define VELOCITY 10

class PlayerModel : public Model {
   protected:
    virtual void init_uniforms() override;
    virtual void init_values() override;

    std::vector<Point> points;
    std::shared_ptr<LineModel> line_model;

    int id;
    Control control;
    std::array<GLubyte, 3> color;

   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLfloat start_pos_y_loc;
    GLfloat start_pos_x_loc;

    double time;
    Point last_point;
    GLfloat speed_x;
    GLfloat speed_y;
    bool is_alive;

    // The Uniform values
    GLfloat trans_y;
    GLfloat trans_x;
    GLfloat start_pos_y;
    GLfloat start_pos_x;

    PlayerModel(int id, GLfloat x, GLfloat y, std::array<GLubyte, 3> color);
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
    void set_keys(Control control);
    void add_line_model(std::shared_ptr<LineModel> line_model);
    Point get_position();
    int get_id();
    std::vector<Point> get_line_points();
};

#endif