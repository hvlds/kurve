#include "arrow_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "arrow_mesh.hpp"
#include "shader.hpp"
#include "point.hpp"


ArrowModel::ArrowModel(
    glm::vec2 start_position, glm::vec3 color, glm::vec2 direction) {   
#ifdef DEBUG
    std::cout << "---- INIT ArrowModel ----" << std::endl;
#endif    
    this->color = color;
    this->start_pos_x = start_position.x;
    this->start_pos_y = start_position.y;
    this->direction = direction;

    std::string vs_path(STATIC_FILES);
    vs_path.append("/shader/arrow.vs");

    std::string fs_path(STATIC_FILES);
    fs_path.append("/shader/arrow.fs");

    Shader shader(vs_path.c_str(), fs_path.c_str(), &this->shader_id);

    this->init_uniforms();

    auto mesh = std::make_shared<ArrowMesh>(this->color);
    this->mesh = mesh;

    this->init_values();    
}

ArrowModel::~ArrowModel() {
#ifdef DEBUG
    std::cout << "---- DESTROY ArrowModel ----" << std::endl;
#endif
    glDeleteProgram(this->shader_id);
    gl_check_error("glDeleteProgram");
}

void ArrowModel::draw() {
    if (this->is_active == true) {
        glUseProgram(this->shader_id);
        this->mesh->draw();
    }
}    

void ArrowModel::update(GLFWwindow* window) {
    auto user_data = (user_data_t*) glfwGetWindowUserPointer(window);
    if (user_data->game_state ==  GAME_TRANSITION) {
        this->is_active = true;
    } else {
        this->is_active = false;
    }
}

void ArrowModel::init_uniforms() {
#ifdef DEBUG
    std::cout << "Init uniforms" << std::endl;
#endif
    // Angle for the rotation
    this->angle_loc = glGetUniformLocation(this->shader_id, "angle");
    gl_check_error("glGetUniformLocation [angle]");
    check_error(this->angle_loc >= 0, "Failed to obtain uniform location for angle.");

    // Start pos Y:
    this->start_pos_y_loc = glGetUniformLocation(this->shader_id, "start_pos_y");
    gl_check_error("glGetUniformLocation [start_pos_y]");
    check_error(this->start_pos_y_loc >= 0, "Failed to obtain uniform location for start_pos_y.");

    // Start pos X:
    this->start_pos_x_loc = glGetUniformLocation(this->shader_id, "start_pos_x");
    gl_check_error("glGetUniformLocation [start_pos_x]");
    check_error(this->start_pos_x_loc >= 0, "Failed to obtain uniform location for start_pos_x.");
}

void ArrowModel::init_values() {
#ifdef DEBUG
    std::cout << "Init values" << std::endl;
#endif
    glm::vec2 pos {this->start_pos_x, this->start_pos_y};
    glm::vec2 pos2 = pos + this->direction * 100.0f;
    this->angle = static_cast<GLfloat>(get_angle(pos, pos2));

    glUniform1f(this->angle_loc, this->angle);
    gl_check_error("glUniform1f [angle]");

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

void ArrowModel::set_direction(glm::vec2 direction) {
    this->direction = direction;
    glm::vec2 pos {this->start_pos_x, this->start_pos_y};
    glm::vec2 pos2 = pos + this->direction * 100.0f;
    this->angle = static_cast<GLfloat>(get_angle(pos, pos2));

    glUniform1f(this->angle_loc, this->angle);
    gl_check_error("glUniform1f [angle]");

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

void ArrowModel::set_position(glm::vec2 position) {
    glUseProgram(this->shader_id);
    this->start_pos_x = position.x;
    this->start_pos_y = position.y;

    glUniform1f(this->angle_loc, this->angle);
    gl_check_error("glUniform1f [angle]");

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

