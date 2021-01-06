#include "arrow_model.hpp"

#include <iostream>

#include "gl_calls.hpp"
#include "arrow_mesh.hpp"
#include "shader.hpp"


ArrowModel::ArrowModel(
    glm::vec2 start_position, glm::vec3 color, glm::vec2 direction) {   
#ifdef DEBUG
    std::cout << "---- INIT ArrowModel ----" << std::endl;
#endif    
    this->color = color;
    this->direction = direction;

    std::string vs_path(STATIC_FILES);
    vs_path.append("/shader/arrow.vs");

    std::string fs_path(STATIC_FILES);
    fs_path.append("/shader/arrow.fs");

    // Compile and add the shaders
    Shader shader(vs_path.c_str(), fs_path.c_str(), &this->shader_id);
    // Init the uniforms
    this->init_uniforms();

    // Add the mesh
    auto mesh = std::make_shared<ArrowMesh>(this->color, this->direction);
    this->mesh = mesh;

    // Init the values of the model
    this->start_pos_x = start_position.x;
    this->start_pos_y = start_position.y;
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
    glUseProgram(this->shader_id);
    this->mesh->draw();
}    

void ArrowModel::update(GLFWwindow* window) {
}

void ArrowModel::init_uniforms() {
#ifdef DEBUG
    std::cout << "Init uniforms" << std::endl;
#endif
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
    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

