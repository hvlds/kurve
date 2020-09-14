#include "renderer.hpp"

void Renderer::draw() {
    for (auto mesh : meshes) {
        mesh.draw();
    }
}