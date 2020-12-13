#include "border_model.hpp"
#include "border_mesh.hpp"

BorderModel::BorderModel() {
    Shader shader("../shader/border.vs", "../shader/border.fs", &this->shader_id);
    
    auto mesh = std::make_shared<BorderMesh>();
    this->mesh = mesh;
}

void BorderModel::update(GLFWwindow* window) {

}

void BorderModel::draw() {
    glUseProgram(this->shader_id);
    this->mesh->draw();
}

void BorderModel::init_uniforms() {

}

void BorderModel::init_values() {

}