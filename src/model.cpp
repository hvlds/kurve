#include "model.hpp"

void Model::draw() {
    for (auto mesh : meshes) {
        mesh.draw();
    }
}