#include "display.hpp"
#include "gl_calls.hpp"
#include "user.hpp"
#include <iostream>

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s (%d)\n", description, error);
	exit(EXIT_FAILURE);
}

void framebuffer_size_callback(GLFWwindow* window, int fb_width, int fb_height)
{
	// glViewport(0, 0, fb_width, fb_height);
	auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
	int width = user_data->window_width;
	int height = user_data->window_height;

	int xoffset, yoffset;
	int new_height, new_width;

	double ratio = fb_width / fb_height;
	if (ratio >= 1) {
		new_width = fb_height;
		new_height = fb_height;
		xoffset = static_cast<int>(round((fb_width - new_width)/2));
		yoffset = 0;
	} else {
		new_width = fb_width;
		new_height = fb_width;
		xoffset = 0;
		yoffset = static_cast<int>(round((fb_height - new_width)/2));
	}

	glViewport(xoffset, yoffset, new_width, new_height);
	gl_check_error("glViewport");
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	user_data->window_width = width;
	user_data->window_height = height;
}

Display::Display(user_data_t* user_data) {
	std::cout << "Creating display" << std::endl;
	
	this->user_data = user_data;

	// Specify our error callback func:
	// Then initialize GLFW itself.
	printf("Initializing GLFW ...\n");

	glfwSetErrorCallback(error_callback);
	check_error(glfwInit(), "Failed to initialize GLFW.");

	// We want at least OpenGL 4.1:
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Enable forward-compatibility and use the core profile:
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFW window:
	printf("Creating window ...\n");

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	this->window = glfwCreateWindow(
        this->user_data->window_width, 
        this->user_data->window_height, 
        "Achtung, die Kurve!", 
        NULL, 
        NULL);
	check_error(this->window != NULL, "Failed to create window.");

	glfwSetWindowSizeLimits(this->window, 700, 700, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// Make the OpenGL context of the window the current one:
	glfwMakeContextCurrent(this->window);

	// Loader function:
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// Try to swap on every screen update:
	glfwSwapInterval(1);

	// Specify remaining callbacks:
	glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(this->window, window_size_callback);

	// Store a pointer to our user data inside the window:
	glfwSetWindowUserPointer(this->window, this->user_data);
}

GLFWwindow* Display::get_window() {
	return this->window;
}
