#include "line_model.hpp"
#include "point.hpp"

LineModel::LineModel(Point point) {
    Shader shader("../shader/player.vs", "../shader/player.fs", &this->shader_id);

    auto mesh = std::make_shared<LineMesh>(point);
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
    this->points.push_back(point);
    this->line_mesh->add_point(point);
}