
#include "player_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "player_mesh.hpp"
#include "shader.hpp"

PlayerModel::PlayerModel() {
    // Compile and add the shaders
    Shader shader("../shader/circle.vs", "../shader/circle.fs", &this->shader_id);

    // Init the uniforms
    this->init_uniforms();

    // Add the mesh
    PlayerMesh mesh;
    this->mesh = mesh;

    // Init the values of the model
    this->init_values();
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

    // X trans:
    this->trans_x_loc = glGetUniformLocation(this->shader_id, "trans_x");
    gl_check_error("glGetUniformLocation [trans_x]");
}

void PlayerModel::init_values() {
    this->time = glfwGetTime();
    this->trans_y = 0;
    this->trans_x = 0;
}
