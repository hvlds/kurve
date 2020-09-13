#include "mesh.hpp"

Mesh::Mesh() {
    
}

int Mesh::get_id() { 
    return this->id;
}

void Mesh::bind() {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
}

void Mesh::draw() {
    this->bind(); // Bind the opengl objects
    glDrawArrays(GL_TRIANGLE_FAN, 0, 200);
    gl_check_error("glDrawElements");
}