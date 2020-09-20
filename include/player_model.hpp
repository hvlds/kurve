#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "model.hpp"
#include "user.hpp"
#include "player_mesh.hpp"
#include "line_model.hpp"
#include "point.hpp"

extern "C" {
// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include <math.h>
}

#include <vector>
#include <array>
#include <memory>

#define VELOCITY 1

class PlayerModel : public Model {
   protected: 
    virtual void init_uniforms() override;
    virtual void init_values() override;
    std::vector<Point> points;
    std::shared_ptr<LineModel> line_model;
    // Controls
    int left_key;
    int right_key;
    int up_key;
    int down_key;
    // color
    std::array<GLubyte, 3> color;
   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLfloat start_pos_y_loc;
    GLfloat start_pos_x_loc;

    double time;

    // The Uniform values
    GLfloat trans_y;
    GLfloat trans_x;
    GLfloat start_pos_y;
    GLfloat start_pos_x;

    PlayerModel(GLfloat x, GLfloat y, std::array<GLubyte, 3> color);
    virtual void update(GLFWwindow* window) override;
    virtual void draw() override;
    void set_keys(int left_key, int right_key, int up_key, int down_key);
    void add_line_model(std::shared_ptr<LineModel> line_model);
};

#endif