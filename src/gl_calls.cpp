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

char* read_shader_source_from_file(const char* path) {
    // Open the file:
    FILE* file = fopen(path, "rb");
    check_error(file != NULL, "Failed to open file.");

    // Seek to the end:
    int success = fseek(file, 0, SEEK_END);
    check_error(success == 0, "Failed to forward file pointer.");

    // Obtain the size of the file from the position of the file pointer:
    long size = ftell(file);
    check_error(size >= 0, "Failed to determine file size.");

    // Rewind the file pointer back to the start:
    rewind(file);

    // Allocate the output buffer:
    char* buffer = (char*)malloc(size + 1);
    check_error(buffer != NULL, "Failed to allocate buffer.");

    // Read into it:
    size_t blocks_read = fread(buffer, size, 1, file);
    check_error(blocks_read == 1, "Failed to read file.");

    // Close the file:
    success = fclose(file);
    check_error(success == 0, "Failed to close file.");

    // Append a null-terminator and return the buffer:
    buffer[size] = '\0';

    return buffer;
}

GLuint compile_shader(GLenum type, const char* shader_path, const char* shader_tag) {
    // Create an empty shader:
    GLuint shader = glCreateShader(type);
    gl_check_error("glCreateShader");

    // Read and specify the source code:
    char* shader_source = read_shader_source_from_file(shader_path);

    glShaderSource(shader, 1, (const char**)&shader_source, NULL);
    gl_check_error("glShaderSource");

    free(shader_source);

    // Compile the shader:
    glCompileShader(shader);
    gl_check_error("glCompileShader");

    // Check the compilation status:
    GLint success;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    gl_check_error("glGetShaderiv");

    if (success) {
        return shader;
    }

    // Extract the length of the error message (including '\0'):
    GLint info_length = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
    gl_check_error("glGetShaderiv");

    if (info_length > 1) {
        // Extract the message itself:
        char* info = (char*)malloc(info_length);
        check_error(info != NULL, "Failed to allocate memory for error message.");

        glGetShaderInfoLog(shader, info_length, NULL, info);
        gl_check_error("glGetShaderInfoLog");

        fprintf(stderr, "Error compiling shader (%s): %s\n", shader_tag, info);
        free(info);
    } else {
        fprintf(stderr, "No info log from the shader compiler :(\n");
    }

    exit(EXIT_FAILURE);
}

void init_shader_program(user_data_t* user_data) {
    // Create the vertex shader:
    printf("Compiling vertex shader ...\n");
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, "../shader/circle.vs", "Vertex shader");

    // Create the fragment shader:
    printf("Compiling fragment shader ...\n");
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "../shader/circle.fs", "Fragment shader");

    // Create an empty shader program:
    printf("Creating shader program ...\n");

    GLuint shader_program = glCreateProgram();
    gl_check_error("glCreateProgram");
    std::cout << "Shader Program ID: " << shader_program << std::endl;

    // Attach both shaders to the program:
    glAttachShader(shader_program, vertex_shader);
    gl_check_error("glAttachShader [vertex]");

    glAttachShader(shader_program, fragment_shader);
    gl_check_error("glAttachShader [fragment]");

    // Link the shader program:
    glLinkProgram(shader_program);
    gl_check_error("glLinkProgram");

    // Detach the shaders after linking:
    glDetachShader(shader_program, vertex_shader);
    gl_check_error("glDetachShader [vertex]");

    glDetachShader(shader_program, fragment_shader);
    gl_check_error("glDetachShader [fragment]");

    // Delete the shaders:
    glDeleteShader(vertex_shader);
    gl_check_error("glDeleteShader [vertex]");

    glDeleteShader(fragment_shader);
    gl_check_error("glDeleteShader [fragment]");

    // Check the link status:
    GLint success;

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    gl_check_error("glGetProgramiv");

    if (success) {
        // Use the program from now on:
        glUseProgram(shader_program);
        gl_check_error("glUseProgram");

        // Store it inside our user data struct:
        user_data->shader_program = shader_program;

        // We can now release the shader compiler.
        glReleaseShaderCompiler();
        gl_check_error("glReleaseShaderCompiler");

        return;
    }

    // Extract the length of the error message (including '\0'):
    GLint info_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_length);
    gl_check_error("glGetProgramiv");

    if (info_length > 1) {
        // Extract the message itself:
        char* info = (char*)malloc(info_length);
        check_error(info != NULL, "Failed to allocate memory for error message.");

        glGetProgramInfoLog(shader_program, info_length, NULL, info);
        gl_check_error("glGetProgramInfoLog");

        fprintf(stderr, "Error linking shader program: %s\n", info);
        free(info);
    } else {
        fprintf(stderr, "No info log from the shader compiler :(\n");
    }

    exit(EXIT_FAILURE);
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

void init_circle_vertex_data(user_data_t* user_data) {
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
            {.position = {static_cast<float>(x), static_cast<float>(y), 0}, .color = {0xFF, 0xFF, 0x00}});
    }

    std::cout << "Vertex in circle: " << vertex_data.size() << std::endl;

    user_data->vertex_data_count += 4;

    // TODO: blackbox! Create a VAO.
    GLuint vao;

    glGenVertexArrays(1, &vao);
    gl_check_error("glGenVertexArrays");

    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    std::cout << "VAO: " << vao << std::endl;

    // Generate and bind an element buffer object:
    // GLuint ebo;
    // glGenBuffers(1, &ebo);

    // GLuint elements[] = {
    //     0, 1, 2,
    //     2, 3, 0
    // };

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Generate and bind a vertex buffer object:
    GLuint vbo;

    glGenBuffers(1, &vbo);
    gl_check_error("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");

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

    // Generate and bind a uniform buffer object:
    GLuint block_index = glGetUniformBlockIndex(
        user_data->shader_program, "triangleBlock");
    GLuint ubo;
    glGenBuffers(1, &ubo);
    GLfloat zoom = 1.0;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat), &zoom, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Store the VBO inside our user data:
    gl_obj_t meta_obj = {
        vao,
        vbo,
        // ebo,
        ubo};
    user_data->vec_obj.push_back(meta_obj);
}

void init_vertex_data(user_data_t* user_data) {
    int circle_count = 4;
    for (int i = 0; i < circle_count; i++) {
        init_circle_vertex_data(user_data);
    }
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

    // Initialize our shader program:
    init_shader_program(user_data);

    // Initialize our uniforms:
    init_uniforms(user_data);

    // Initialize our model:
    init_model(user_data);

    // Initialize our vertex data:
    init_circle_vertex_data(user_data);
    init_circle_vertex_data(user_data);

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
    std::cout << user_data->trans_y_loc << std::endl;

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
