#include "line_mesh.hpp"

#include <iostream>
#include <cmath>

void LineMesh::update() {
    if (this->points.size() > 0 ) {
        this->bind();
        std::vector<vertex_data_t> vertex_data;

        for (auto point : this->points) {        
            vertex_data.push_back(
                {
                    .position = {
                        static_cast<GLfloat>(point.x), 
                        static_cast<GLfloat>(point.y), 
                        0
                    }, 
                    .color = {this->color[0], this->color[1], this->color[2]}
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
}

void LineMesh::set_points(std::vector<Point> points) {
    this->points = points;
}

void LineMesh::add_point(Point point) {
    if (this->line_points.size() == 0) {
        this->line_points.push_back(point);
    } else if (this->line_points.size() == 1) {
        this->last_point = this->line_points.back();
        this->line_points.push_back(point);
    } else {
        this->second_last_point = this->last_point;
        this->last_point = this->line_points.back();

        Vector v1{this->last_point, this->second_last_point}; // second last - last
        Vector v2{this->last_point, point}; // actual - last
        
        double angle = Vector::angle(v1, v2);
        // std::cout << "angle: " << angle << std::endl;
        double direction = Vector::cross_product(v1, v2);
        direction = round(direction);
        // std::cout << "direction: " << direction << std::endl;

        GLfloat r{0.1};
        Point left_point, right_point, left_diff, right_diff;

        double v2_length = v2.get_length();
        v2.x = (v2.x / (GLfloat) v2_length) * r;
        v2.y = (v2.y / (GLfloat) v2_length) * r;

        if (direction < 0) {
            // Rotation clockwise
            angle = angle / 2;
            left_point = {
                v2.x * (GLfloat)cos(M_PI - angle) - v2.y * (GLfloat)sin(M_PI - angle),
                v2.x * (GLfloat)sin(M_PI - angle) + v2.y * (GLfloat)cos(M_PI - angle)};
            right_point = {
                v2.x * (GLfloat)cos(-angle) - v2.y * (GLfloat)sin(-angle),
                v2.x * (GLfloat)sin(-angle) + v2.y * (GLfloat)cos(-angle)};

        } else if (direction > 0) {
            // Rotation anti-clockwise
            angle = angle / 2;
            left_point = {
                v2.x * (GLfloat)cos(angle) - v2.y * (GLfloat)sin(angle),
                v2.x * (GLfloat)sin(angle) + v2.y * (GLfloat)cos(angle)};
            right_point = {
                v2.x * (GLfloat)cos(-M_PI + angle) - v2.y * (GLfloat)sin(-M_PI + angle),
                v2.x * (GLfloat)sin(-M_PI + angle) + v2.y * (GLfloat)cos(-M_PI + angle)};
        } else {
            angle = M_PI/2;
            left_point = {
                v2.x * (GLfloat)cos(angle) - v2.y * (GLfloat)sin(angle),
                v2.x * (GLfloat)sin(angle) + v2.y * (GLfloat)cos(angle)};
            right_point = {
                v2.x * (GLfloat)cos(-angle) - v2.y * (GLfloat)sin(-angle),
                v2.x * (GLfloat)sin(-angle) + v2.y * (GLfloat)cos(-angle)};
        }

        left_point.x += this->last_point.x;
        left_point.y += this->last_point.y;

        right_point.x += this->last_point.x;
        right_point.y += this->last_point.y;

        // std::cout << "Diameter: " << Point::get_distance(left_point, right_point) << std::endl;

        this->line_points.push_back(point);
        this->points.push_back(left_point);
        this->points.push_back(right_point);
    }

}

LineMesh::LineMesh(Point first_point, std::array<GLubyte, 3> color) {
    this->color = color;
    
    std::vector<vertex_data_t> vertex_data;
    vertex_data.push_back(
        {
            .position = {
                static_cast<GLfloat>(first_point.x), 
                static_cast<GLfloat>(first_point.y), 
                0
            }, 
            .color = {this->color[0], this->color[1], this->color[2]}
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
    if (this->points.size() > 0) {
        this->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, this->points.size());
        gl_check_error("glDrawArrays");
    }
}

void LineMesh::clear() {
    this->line_points.clear();
    this->points.clear();   
}