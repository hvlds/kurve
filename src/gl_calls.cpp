#include "gl_calls.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "mesh.hpp"
#include "user.hpp"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "obj.h"
#include "bitmap.h"
}

#define MODEL_PATH "./models/ring"    // Doesn't exists!
#define TEX_PATH "./models/logo.bmp"  // Doesn't exists!
#define Y_ANGULAR_VELOCITY 2

void check_error(int condition, const char* error_text) {
    if (!condition) {
        fprintf(stderr, "%s\n", error_text);
        exit(EXIT_FAILURE);
    }
}

void gl_check_error(const char* error_text) {
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        fprintf(stderr, "GLError: %s (%d)\n", error_text, error);
        exit(EXIT_FAILURE);
    }
}

void init_gl(GLFWwindow* window) {
    std::cout << "init_gl" << std::endl;
    
    // Obtain the internal size of the framebuffer:
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // Align the viewport to the framebuffer:
    glViewport(0, 0, fb_width, fb_height);
    gl_check_error("glViewport");

    // Specify the clear color:
    glClearColor(0.1, 0.1, 0.1, 1);
    gl_check_error("glClearColor");
}
