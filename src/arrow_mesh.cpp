#include "arrow_mesh.hpp"

#include <iostream>

extern "C" {
#include <math.h>
#include <stdlib.h>
}

ArrowMesh::ArrowMesh(glm::vec3 color, glm::vec2 direction) {
    // Triangle data:
    std::vector<vertex_data_t> vertex_data {
        {
            .position = {static_cast<GLfloat>(-0.25), static_cast<GLfloat>(-1), 0},
            .color = color
        },
        {
            .position = {static_cast<GLfloat>(0.25), static_cast<GLfloat>(-1), 0}, 
            .color = color
        }, 
        {   .position = {static_cast<GLfloat>(0.25), static_cast<GLfloat>(0), 0}, 
            .color = color
        },

        {
            .position = {static_cast<GLfloat>(-0.25), static_cast<GLfloat>(-1), 0},
            .color = color
        },
        {   .position = {static_cast<GLfloat>(0.25), static_cast<GLfloat>(0), 0}, 
            .color = color
        },
        {
            .position = {static_cast<GLfloat>(-0.25), static_cast<GLfloat>(0), 0}, 
            .color = color
        },

        {   .position = {static_cast<GLfloat>(-0.5), static_cast<GLfloat>(0), 0}, 
            .color = color
        },  
        {   .position = {static_cast<GLfloat>(0.5), static_cast<GLfloat>(0), 0}, 
            .color = color
        }, 
        {   .position = {static_cast<GLfloat>(0), static_cast<GLfloat>(1), 0}, 
            .color = color
        }, 
    };

    // TODO: blackbox! Create a VAO.
    GLuint vao;

    glGenVertexArrays(1, &vao);
    gl_check_error("glGenVertexArrays");

    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    this->vao = vao;

    // Generate and bind a vertex buffer object:
    GLuint vbo;

    glGenBuffers(1, &vbo);
    gl_check_error("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");
    
    this->vbo = vbo;

    // Upload the vertex data to the GPU:
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertex_data.size() * sizeof(vertex_data_t), 
        (const GLvoid*)vertex_data.data(), 
        GL_STATIC_DRAW);
    gl_check_error("glBufferData");

    // Position attribute:
    // Number of attribute, number of components, 
    // data type, normalize, stride, pointer (= offset)
    glVertexAttribPointer(
        ATTRIB_POSITION, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(vertex_data_t), 
        (GLvoid*)offsetof(vertex_data_t, position));
    gl_check_error("glVertexAttribPointer [position]");

    glVertexAttribPointer(
        ATTRIB_COLOR, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(vertex_data_t), 
        (GLvoid*)offsetof(vertex_data_t, color));
    gl_check_error("glVertexAttribPointer [color]");

    // Enable the attributes:
    glEnableVertexAttribArray(ATTRIB_POSITION);
    gl_check_error("glEnableVertexAttribArray [position]");

    glEnableVertexAttribArray(ATTRIB_COLOR);
    gl_check_error("glEnableVertexAttribArray [color]");
}

ArrowMesh::~ArrowMesh() {
#ifdef DEBUG
    std::cout << "---- DESTROY ArrowMesh ----" << std::endl;
#endif
    glDeleteVertexArrays(1, &this->vao);
    gl_check_error("glDeleteVertexArrays");

    glDeleteBuffers(1, &this->vbo);
    gl_check_error("glDeleteBuffers");
}

void ArrowMesh::draw() {
    std::cout << "Draw arrow mesh" << std::endl;
    this->bind();
    glDrawArrays(GL_TRIANGLES, 0, 9);
    gl_check_error("glDrawArrays");
}