#include "player_mesh.hpp"

#include <iostream>

extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"
#include "obj.h"
}

PlayerMesh::PlayerMesh() {
    // Triangle data:
    std::vector<vertex_data_t> vertex_data;

    int parts{200};
    double radius{1};
    double delta_angle{(2 * 3.14159265) / parts};
    double angle{0};

    for (int i = 0; i < parts; i++) {
        double x{radius * cos(angle)};
        double y{radius * sin(angle)};
        angle += delta_angle;
        vertex_data.push_back(
            {.position = {static_cast<float>(x), static_cast<float>(y), 0}, .color = {0xFF, 0x00, 0xFF}});
    }

    // TODO: blackbox! Create a VAO.
    GLuint vao;

    glGenVertexArrays(1, &vao);
    gl_check_error("glGenVertexArrays");

    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    this->vao = vao;
    std::cout << "VAO: " << vao << std::endl;

    // Generate and bind a vertex buffer object:
    GLuint vbo;

    glGenBuffers(1, &vbo);
    gl_check_error("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");
    
    this->vbo = vbo;
    std::cout << "VBO: " << vbo << std::endl;

    // Upload the vertex data to the GPU:
    glBufferData(GL_ARRAY_BUFFER, parts * sizeof(vertex_data_t), (const GLvoid*)vertex_data.data(), GL_STATIC_DRAW);
    gl_check_error("glBufferData");

    // Position attribute:
    // Number of attribute, number of components, data type, normalize, stride, pointer (= offset)
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, position));
    gl_check_error("glVertexAttribPointer [position]");

    glVertexAttribPointer(ATTRIB_COLOR, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, color));
    gl_check_error("glVertexAttribPointer [color]");

    // Enable the attributes:
    glEnableVertexAttribArray(ATTRIB_POSITION);
    gl_check_error("glEnableVertexAttribArray [position]");

    glEnableVertexAttribArray(ATTRIB_COLOR);
    gl_check_error("glEnableVertexAttribArray [color]");
    
}

void PlayerMesh::draw() {
    this->bind();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 200);
    gl_check_error("glDrawArrays");
}