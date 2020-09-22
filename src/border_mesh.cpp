#include "border_mesh.hpp"
#include "point.hpp"
#include <iostream>

BorderMesh::BorderMesh() {
    // Triangle data:
    std::vector<vertex_data_t> vertex_data;

    std::vector<Point> points = {
        {0.9, 0.9}, {-0.9, 0.9}, {-0.9, -0.9}, {0.9, -0.9}
    };

    for (auto point : points) {
        vertex_data.push_back(
            {
                .position = {
                    static_cast<GLfloat>(point.x), 
                    static_cast<GLfloat>(point.y), 
                    0
                }, 
                .color = {0xFF, 0xFF, 0x00}
            }
        );
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

void BorderMesh::draw() {
    this->bind();
    glDrawArrays(GL_LINE_LOOP, 0 , 4);
    gl_check_error("glDrawArrays");
}