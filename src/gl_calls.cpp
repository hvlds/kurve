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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    if (action == GLFW_RELEASE) {
        return;
    }

    if (key == GLFW_KEY_S) {
        switch (user_data->shading_case) {
            case 0:
            case 1:
                user_data->shading_case += 1;
                break;
            case 2:
                user_data->shading_case = 0;
                break;
            default:
                user_data->shading_case = 0;
        }
    }
}

void init_texture(user_data_t* user_data) {
    // Activate the first texture unit:
    glActiveTexture(GL_TEXTURE0);
    gl_check_error("glActiveTexture");

    // Generate a new texture object:
    GLuint tex;

    glGenTextures(1, &tex);
    gl_check_error("glGenTextures");

    // Bind it to the 2D texture slot:
    glBindTexture(GL_TEXTURE_2D, tex);
    gl_check_error("glBindTexture");

    // Specify wrapping (uv <=> st):
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_check_error("glTexParameteri [wrap_s]");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_check_error("glTexParameteri [wrap_t]");

    // Specify filtering:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_check_error("glTexParameteri [min_filter]");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl_check_error("glTexParameteri [mag_filter]");

    // Load the bitmap:
    bitmap_pixel_rgb_t* pixels;
    int width, height;

    bitmap_error_t err = bitmapReadPixels(TEX_PATH, (bitmap_pixel_t**)&pixels, &width, &height, BITMAP_COLOR_SPACE_RGB);
    check_error(err == BITMAP_ERROR_SUCCESS, "Failed to load texture bitmap.");

    // Upload the texture pixels to the GPU:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    gl_check_error("glTexImage2D");

    // Free the pixels and store the texture handle:
    free(pixels);
    user_data->tex = tex;
}

void init_uniforms(user_data_t* user_data) {
    // Y trans:
    user_data->trans_y_loc = glGetUniformLocation(user_data->shader_program, "trans_y");
    gl_check_error("glGetUniformLocation [trans_y]");
    //check_error(user_data->angle_y_loc >= 0, "Failed to obtain uniform location for angle_y.");

    // X trans:
    user_data->trans_x_loc = glGetUniformLocation(user_data->shader_program, "trans_x");
    gl_check_error("glGetUniformLocation [trans_x]");
    //check_error(user_data->angle_x_loc >= 0, "Failed to obtain uniform location for angle_x.");

    // Trans
    user_data->trans_loc = glGetUniformLocation(user_data->shader_program, "trans");
    gl_check_error("glGetUniformLocation [trans]");

    // Trans_vector_x
    user_data->trans_vec_x_loc = glGetUniformLocation(user_data->shader_program, "trans_vec_x");
    gl_check_error("glGetUniformLocation [trans_vec_x]");

    // Trans_vector_y
    user_data->trans_vec_y_loc = glGetUniformLocation(user_data->shader_program, "trans_vec_y");
    gl_check_error("glGetUniformLocation [trans_vec_y]");
}

void init_model(user_data_t* user_data) {
    user_data->time = glfwGetTime();
    user_data->trans_y = 0;
    user_data->trans_x = 0;

    GLfloat v_x[4] = {1, -1, -1, 1};
    GLfloat v_y[4] = {1, -1, 1, -1};
    // glUniform1fv(glGetUniformLocation(user_data->shader_program, "trans_vec[0]"), 4, v);
    glUniform1fv(user_data->trans_vec_x_loc, 4, v_x);
    glUniform1fv(user_data->trans_vec_y_loc, 4, v_y);
}

void init_gl(GLFWwindow* window) {
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    // Initialize our uniforms:
    init_uniforms(user_data);

    // Initialize our model:
    init_model(user_data);

    // Obtain the internal size of the framebuffer:
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // Align the viewport to the framebuffer:
    glViewport(0, 0, fb_width, fb_height);
    gl_check_error("glViewport");

    // Specify the clear color:
    glClearColor(0.1, 0.1, 0.1, 1);
    gl_check_error("glClearColor");

    // Init the Key Callback for the change of shading
    glfwSetKeyCallback(window, key_callback);
}

void update_gl(GLFWwindow* window) {
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    // Update the time and calculate the delta:
    double new_time = glfwGetTime();
    double time_delta = new_time - user_data->time;

    user_data->time = new_time;

    int right_state = glfwGetKey(window, GLFW_KEY_LEFT);
    if (right_state == GLFW_PRESS) {
        user_data->trans_x = fmod(user_data->trans_x + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int left_state = glfwGetKey(window, GLFW_KEY_RIGHT);
    if (left_state == GLFW_PRESS) {
        user_data->trans_x = fmod(user_data->trans_x + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int up_state = glfwGetKey(window, GLFW_KEY_DOWN);
    if (up_state == GLFW_PRESS) {
        user_data->trans_y = fmod(user_data->trans_y + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    int down_state = glfwGetKey(window, GLFW_KEY_UP);
    if (down_state == GLFW_PRESS) {
        user_data->trans_y = fmod(user_data->trans_y + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
    }

    // Update the uniform:
    glUniform1f(user_data->trans_y_loc, user_data->trans_y);
    gl_check_error("glUniform1f [trans_y]");

    glUniform1f(user_data->trans_x_loc, user_data->trans_x);
    gl_check_error("glUniform1f [trans_x]");
}

void draw_gl(GLFWwindow* window) {
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    // Clear the color buffer -> background color:
    glClear(GL_COLOR_BUFFER_BIT);
    gl_check_error("glClear");

    // Draw our stuff!
    for (auto meta_obj : user_data->vec_obj) {
        glBindVertexArray(meta_obj.vao);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meta_obj.ebo);
        glBindBuffer(GL_ARRAY_BUFFER, meta_obj.vbo);
        glBindBuffer(GL_UNIFORM_BUFFER, meta_obj.ubo);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 2);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 200, 4);
        gl_check_error("glDrawElements");
    }
}

void teardown_gl(GLFWwindow* window) {
    printf("Tearing down ...\n");
    user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    // Delete the shader program:
    glDeleteProgram(user_data->shader_program);
    gl_check_error("glDeleteProgram");

    // Delete the VAO:
    for (auto meta_obj : user_data->vec_obj) {
        glDeleteVertexArrays(1, &meta_obj.vao);
        gl_check_error("glDeleteVertexArrays");

        // glDeleteBuffers(1, &meta_obj.ebo);
        // gl_check_error("glDeleteBuffers");

        glDeleteBuffers(1, &meta_obj.vbo);
        gl_check_error("glDeleteBuffers");
    }
}
