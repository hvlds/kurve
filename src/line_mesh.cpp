#include "line_mesh.hpp"

#include <iostream>

void LineMesh::update() {
    this->bind();
    std::vector<vertex_data_t> vertex_data;

    for (auto point : this->points) {
        vertex_data.push_back(
            {
                .position = {
                    static_cast<float>(point.x), 
                    static_cast<float>(point.y), 
                    0
                }, 
                .color = {0xFF, 0x00, 0x00}
            }
        );
    }

    // Upload the vertex data to the GPU:
    glBufferData(
        GL_ARRAY_BUFFER, 
        this->points.size() * sizeof(vertex_data_t), 
        (const GLvoid*)vertex_data.data(), 
        GL_DYNAMIC_DRAW);
    gl_check_error("glBufferData");

}

void LineMesh::set_points(std::vector<Point> points) {
    this->points = points;
}

void LineMesh::add_point(Point point) {
    this->points.push_back(point);
}

LineMesh::LineMesh(Point first_point) {
    std::vector<vertex_data_t> vertex_data;
    vertex_data.push_back(
        {
            .position = {
                static_cast<float>(first_point.x/10), 
                static_cast<float>(first_point.y/10), 
                0
            }, 
            .color = {0xFF, 0x00, 0x00}
        }
    );

    // Create a VAO
    GLuint vao;

    glGenVertexArrays(1, &vao);
    gl_check_error("glGenVertexArrays");

    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    this->vao = vao;
    std::cout << "VAO: " << vao << std::endl;

    // Generate and bind a vertex buffer object (VBO):
    GLuint vbo;

    glGenBuffers(1, &vbo);
    gl_check_error("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");
    
    this->vbo = vbo;
    std::cout << "VBO: " << vbo << std::endl;

    // Upload the vertex data to the GPU:
    glBufferData(
        GL_ARRAY_BUFFER, 
        this->points.size() * sizeof(vertex_data_t), 
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
        GL_UNSIGNED_BYTE, 
        GL_TRUE, 
        sizeof(vertex_data_t), 
        (GLvoid*)offsetof(vertex_data_t, color));
    gl_check_error("glVertexAttribPointer [color]");

    // Enable the attributes:
    glEnableVertexAttribArray(ATTRIB_POSITION);
    gl_check_error("glEnableVertexAttribArray [position]");

    glEnableVertexAttribArray(ATTRIB_COLOR);
    gl_check_error("glEnableVertexAttribArray [color]");

}

void LineMesh::draw() {
    this->bind();
    glDrawArrays(GL_LINE_STRIP, 0, this->points.size());
    gl_check_error("glDrawArrays");
}