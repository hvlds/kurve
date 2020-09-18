#ifndef MESH_HPP
#define MESH_HPP

#include "gl_calls.hpp"

extern "C" {
	// Include the GLAD loader *before* including GLFW!
	#include "glad/glad.h"
	
	// Include the GLFW library (should be the same for all OS):
	#include <GLFW/glfw3.h>
}

class Mesh {
   protected:
    GLuint vao; // Vertex array object
	GLuint vbo; // Vertex buffer object
    int id;
    void bind() {
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    }
   public:
    void terminate() {
        glDeleteVertexArrays(1, &this->vao);
        gl_check_error("glDeleteVertexArrays");

        glDeleteBuffers(1, &this->vbo);
        gl_check_error("glDeleteBuffers");
    }
    void draw() {};
    void init();
};

typedef struct
{
	GLfloat position[3];
	GLubyte color[3];
} vertex_data_t;

#endif