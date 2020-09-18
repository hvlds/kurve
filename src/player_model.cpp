
#include "player_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

PlayerModel::PlayerModel(GLfloat x, GLfloat y) {
    // Compile and add the shaders
    Shader shader("../shader/circle.vs", "../shader/circle.fs", &this->shader_id);
    // Init the uniforms
    this->init_uniforms();

    // Add the mesh
    auto player_mesh = std::make_shared<PlayerMesh>();
    this->mesh = player_mesh;

    // Init the values of the model
    this->start_pos_x = x;
    this->start_pos_y = y;
    this->init_values();
}

void PlayerModel::draw() {
    glUseProgram(this->shader_id);
    this->mesh->draw();
}    

void PlayerModel::update(GLFWwindow* window) {
    // Update the time and calculate the delta:
    double new_time = glfwGetTime();
    double time_delta = new_time - this->time;

    this->time = new_time;

    int right_state = glfwGetKey(window, GLFW_KEY_LEFT);
    if (right_state == GLFW_PRESS) {
        this->trans_x = fmod(this->trans_x + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int left_state = glfwGetKey(window, GLFW_KEY_RIGHT);
    if (left_state == GLFW_PRESS) {
        this->trans_x = fmod(this->trans_x + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int up_state = glfwGetKey(window, GLFW_KEY_DOWN);
    if (up_state == GLFW_PRESS) {
        this->trans_y = fmod(this->trans_y + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int down_state = glfwGetKey(window, GLFW_KEY_UP);
    if (down_state == GLFW_PRESS) {
        this->trans_y = fmod(this->trans_y + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    // Update the uniform:
    glUniform1f(this->trans_y_loc, this->trans_y);
    gl_check_error("glUniform1f [trans_y]");

    glUniform1f(this->trans_x_loc, this->trans_x);
    gl_check_error("glUniform1f [trans_x]");
}

void PlayerModel::init_uniforms() {
    std::cout << "Init uniforms" << std::endl;
    // Y trans:
    this->trans_y_loc = glGetUniformLocation(this->shader_id, "trans_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    check_error(this->trans_y_loc >= 0, "Failed to obtain uniform location for trans_y.");

    // X trans:
    this->trans_x_loc = glGetUniformLocation(this->shader_id, "trans_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    check_error(this->trans_x_loc >= 0, "Failed to obtain uniform location for trans_x.");

    // Start pos Y:
    this->start_pos_y_loc = glGetUniformLocation(this->shader_id, "start_pos_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    check_error(this->start_pos_y_loc >= 0, "Failed to obtain uniform location for start_pos_y.");

    // Start pos X:
    this->start_pos_x_loc = glGetUniformLocation(this->shader_id, "start_pos_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    check_error(this->start_pos_x_loc >= 0, "Failed to obtain uniform location for start_pos_x.");
}

void PlayerModel::init_values() {
    this->time = glfwGetTime();
    this->trans_y = 0;
    this->trans_x = 0;

    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}
