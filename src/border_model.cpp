#include "border_model.hpp"
#include "border_mesh.hpp"

BorderModel::BorderModel() {
    std::string vs_path(STATIC_FILES);
    vs_path.append("/shader/border.vs");

    std::string fs_path(STATIC_FILES);
    fs_path.append("/shader/border.fs");

    Shader shader(vs_path.c_str(), fs_path.c_str(), &this->shader_id);
    
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