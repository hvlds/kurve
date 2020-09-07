#ifndef GLCALLS_HPP
#define GLCALLS_HPP

extern "C" {
	// Include the GLAD loader *before* including GLFW!
	#include "glad/glad.h"
	
	// Include the GLFW library (should be the same for all OS):
	#include <GLFW/glfw3.h>
}
#include <vector>

typedef struct {
	GLuint vao; // Vertex array object
	GLuint vbo; // Vertex buffer object
	GLuint ebo; // Element buffer object
	GLuint ubo; // Uniform buffer object
} gl_obj_t; 


typedef struct {
	// Dimensions of the window:
	int window_width;
	int window_height;

	// The current shader program:
	GLuint shader_program;

	// The uniform locations:
	GLint angle_y_loc;
	GLint angle_x_loc;

	std::vector<gl_obj_t> vec_obj;

	// The texture object:
	GLuint tex;

	// The number of vertices to render:
	int vertex_data_count;

	// The model:
	double time;
	GLfloat angle_y;
	GLfloat angle_x;
	GLuint shading_case;
	int count = 0;

} user_data_t;

typedef struct
{
	GLfloat position[3];
	GLubyte color[3];
} vertex_data_t;

// Vertex attributes:
#define ATTRIB_POSITION 0
#define ATTRIB_COLOR 1

// Generic error checks:
void check_error(int condition, const char* error_text);
void gl_check_error(const char* error_text);

// GL functions:
void init_gl(GLFWwindow* window);
void update_gl(GLFWwindow* window);
void draw_gl(GLFWwindow* window);
void teardown_gl(GLFWwindow* window);

#endif
