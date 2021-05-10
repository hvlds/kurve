#ifndef PLAYER_MODEL_HPP
#define PLAYER_MODEL_HPP

#include "line_model.hpp"
#include "arrow_model.hpp"
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

    std::vector<glm::vec2> points;
    std::vector<std::shared_ptr<LineModel>> lines;
    std::shared_ptr<ArrowModel> arrow; 
    int blank_count = 0;
    int random_length;

    int id;
    Control control;
    glm::vec3 color;

   public:
    // The uniforms locations
    GLint trans_y_loc;
    GLint trans_x_loc;
    GLfloat start_pos_y_loc;
    GLfloat start_pos_x_loc;

    double time;
    glm::vec2 last_point;
    GLfloat speed_x;
    GLfloat speed_y;
    bool is_alive;

    // The Uniform values
    GLfloat trans_y;
    GLfloat trans_x;
    GLfloat start_pos_y;
    GLfloat start_pos_x;

    PlayerModel(GLFWwindow* window, int id, GLfloat x, GLfloat y, glm::vec3 color);
    ~PlayerModel();
    virtual void update(GLFWwindow* window) override;
    virtual void draw(GLFWwindow* window) override;
    
    /**
     * @brief Set the keys to control the player
     * 
     * @param control A control struct with the right and left keys
     */
    void set_keys(Control control);
    glm::vec2 get_position();
    void set_position(glm::vec2 point);
    int get_id();

    /**
     * @brief Get the points of the multiple lines of the player
     * 
     * @return std::vector<Point> A vector with all the points
     */
    std::vector<glm::vec2> get_line_points();

    /**
     * @brief Remove all the stored points and lines of the model
     * and set all the fields into their initial values.
     * 
     */
    virtual void clear();
};

#endif