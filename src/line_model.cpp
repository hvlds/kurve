#include "line_model.hpp"
#include "point.hpp"
#include <iostream>

LineModel::LineModel(Point point, std::array<GLubyte, 3> color) {
    this->model_type = MODEL_LINE;
    this->color = color;

    Shader shader("../shader/player.vs", "../shader/player.fs", &this->shader_id);

    auto mesh = std::make_shared<LineMesh>(point, this->color);
    this->line_mesh = mesh;

    this->points.push_back(point);
}

void LineModel::update(GLFWwindow* window) {
    this->line_mesh->update();
}

void LineModel::draw() {
    glUseProgram(this->shader_id);
    this->line_mesh->draw();
}

void LineModel::init_uniforms() {

}

void LineModel::init_values() {

}

void LineModel::add_point(Point point) {
    auto last_point = this->points.back();
    this->points.push_back(point);
    this->line_mesh->add_point(point);
}

void LineModel::set_color(std::array<GLubyte, 3> color) {
    this->color = color;
}

std::vector<Point> LineModel::get_points() {
    return this->points;
}