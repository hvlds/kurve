#include "line_model.hpp"
#include "point.hpp"
#include <iostream>


LineModel::LineModel(Point point, std::array<GLubyte, 3> color) {
    std::cout << "---- INIT LineModel ----" << std::endl;
    this->color = color;

    Shader shader("../shader/line.vs", "../shader/line.fs", &this->shader_id);
    this->init_uniforms();

    auto mesh = std::make_shared<LineMesh>(point, this->color);
    this->line_mesh = mesh;

    // Init the values of the model
    std::cout << "x: " << point.x << " y: " << point.y << std::endl;
    this->start_pos_x = 0; 
    this->start_pos_y = 0;
    
    this->init_values();

    // this->points.push_back(point);
    this->add_point(point);
}

LineModel::~LineModel() {
    std::cout << "---- DESTROY LineModel ----" << std::endl;
    glDeleteProgram(this->shader_id);
    gl_check_error("glDeleteProgram");
}

void LineModel::update(GLFWwindow* window) {
    glUseProgram(this->shader_id);
    this->line_mesh->update();
}

void LineModel::draw() {
    glUseProgram(this->shader_id);
    this->line_mesh->draw();
}

void LineModel::init_uniforms() {
    std::cout << "Init uniforms" << std::endl;
    // Start pos Y:
    this->start_pos_y_loc = glGetUniformLocation(this->shader_id, "start_pos_y");
    gl_check_error("glGetUniformLocation [start_pos_y]");
    check_error(this->start_pos_y_loc >= 0, "Failed to obtain uniform location for start_pos_y.");

    // Start pos X:
    this->start_pos_x_loc = glGetUniformLocation(this->shader_id, "start_pos_x");
    gl_check_error("glGetUniformLocation [start_pos_x]");
    check_error(this->start_pos_x_loc >= 0, "Failed to obtain uniform location for start_pos_x.");
}

void LineModel::init_values() {
    std::cout << "Init values" << std::endl;
    glUniform1f(this->start_pos_x_loc, this->start_pos_x);
    gl_check_error("glUniform1f [start_pos_x]");

    glUniform1f(this->start_pos_y_loc, this->start_pos_y);
    gl_check_error("glUniform1f [start_pos_y]");
}

void LineModel::add_point(Point point) {
    // if (this->points.size() > 0) {
    //     auto last_point = this->points.back();
    // }
    this->points.push_back(point);
    this->line_mesh->add_point(point);
}

void LineModel::set_color(std::array<GLubyte, 3> color) {
    this->color = color;
}

std::vector<Point> LineModel::get_points() {
    return this->points;
}

void LineModel::clear() {
    this->points.clear();
    line_mesh->clear();
}

void LineModel::set_start_position(Point point) {
    glUseProgram(this->shader_id);
    
    this->add_point(point);
    this->start_pos_x = 0; 
    this->start_pos_y = 0;

    this->init_values();
}