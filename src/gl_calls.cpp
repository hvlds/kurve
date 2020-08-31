#include "gl_calls.hpp"

extern "C" {
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#include "obj.h"
	#include "bitmap.h"
}

#define MODEL_PATH "./models/teapot"
#define TEX_PATH "./models/logo.bmp"
#define Y_ANGULAR_VELOCITY 2

void check_error(int condition, const char* error_text)
{
	if (!condition)
	{
		fprintf(stderr, "%s\n", error_text);
		exit(EXIT_FAILURE);
	}
}

void gl_check_error(const char* error_text)
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
	{
		fprintf(stderr, "GLError: %s (%d)\n", error_text, error);
		exit(EXIT_FAILURE);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	if(action == GLFW_RELEASE) {
		return;	
	} 

    if(key == GLFW_KEY_S) {
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

char* read_shader_source_from_file(const char* path)
{
	// Open the file:
	FILE* file = fopen(path , "rb");
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
	char* buffer = (char*) malloc(size + 1);
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

GLuint compile_shader(GLenum type, const char* shader_path, const char* shader_tag)
{
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

	if (success)
	{
		return shader;
	}

	// Extract the length of the error message (including '\0'):
	GLint info_length = 0;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
	gl_check_error("glGetShaderiv");

	if (info_length > 1)
	{
		// Extract the message itself:
		char* info = (char *) malloc(info_length);
		check_error(info != NULL, "Failed to allocate memory for error message.");

		glGetShaderInfoLog(shader, info_length, NULL, info);
		gl_check_error("glGetShaderInfoLog");

		fprintf(stderr, "Error compiling shader (%s): %s\n", shader_tag, info);
		free(info);
	}
	else
	{
		fprintf(stderr, "No info log from the shader compiler :(\n");
	}

	exit(EXIT_FAILURE);
}

void init_shader_program(user_data_t* user_data)
{
	// Create the vertex shader:
	printf("Compiling vertex shader ...\n");
	GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, "shader/vertex.glsl", "Vertex shader");

	// Create the fragment shader:
	printf("Compiling fragment shader ...\n");
	GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, "shader/fragment.glsl", "Fragment shader");

	// Create an empty shader program:
	printf("Creating shader program ...\n");

	GLuint shader_program = glCreateProgram();
	gl_check_error("glCreateProgram");

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

	if (success)
	{
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

	if (info_length > 1)
	{
		// Extract the message itself:
		char* info = (char*) malloc(info_length);
		check_error(info != NULL, "Failed to allocate memory for error message.");

		glGetProgramInfoLog(shader_program, info_length, NULL, info);
		gl_check_error("glGetProgramInfoLog");

		fprintf(stderr, "Error linking shader program: %s\n", info);
		free(info);
	}
	else
	{
		fprintf(stderr, "No info log from the shader compiler :(\n");
	}

	exit(EXIT_FAILURE);
}

void init_texture(user_data_t* user_data)
{
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

void init_uniforms(user_data_t* user_data)
{
	// Y angle:
	user_data->angle_y_loc = glGetUniformLocation(user_data->shader_program, "angle_y");
	gl_check_error("glGetUniformLocation [angle_y]");
	check_error(user_data->angle_y_loc >= 0, "Failed to obtain uniform location for angle_y.");

	// X angle:
	user_data->angle_x_loc = glGetUniformLocation(user_data->shader_program, "angle_x");
	gl_check_error("glGetUniformLocation [angle_x]");
	check_error(user_data->angle_x_loc >= 0, "Failed to obtain uniform location for angle_x.");

	// Shading case:
	user_data->shading_case_loc = glGetUniformLocation(user_data->shader_program, "shading_case");
	gl_check_error("glGetUniformLocation [shading_case]");
	// check_error(user_data->shading_case_loc >= 0, "Failed to obtain uniform location for shading_case.");

	// Texture:
	GLint tex_loc = glGetUniformLocation(user_data->shader_program, "tex");

	gl_check_error("glGetUniformLocation [tex]");
	check_error(tex_loc >= 0, "Failed to obtain uniform location for tex.");

	// Associate the sampler "tex" with texture unit 0:
	glUniform1i(tex_loc, 0);
	gl_check_error("glUniform1i [tex]");
}

void init_vertex_data(user_data_t* user_data)
{
	// TODO: blackbox! Create a VAO.
	GLuint vao;

	glGenVertexArrays(1, &vao);
	gl_check_error("glGenVertexArrays");

	glBindVertexArray(vao);
	gl_check_error("glBindVertexArray");

	// Store the VAO inside our user data:
	user_data->vao = vao;

	// Generate and bind a vertex buffer object:
	GLuint vbo;

	glGenBuffers(1, &vbo);
	gl_check_error("glGenBuffers");

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	gl_check_error("glBindBuffer");

	// Open the obj file:
	FILE* obj = fopen(MODEL_PATH, "r");
	check_error(obj != NULL, "Failed to open obj file at \"" MODEL_PATH "\".");

	// Count the entries:
	int vertex_count = 0;
	int tex_coords_count = 0;
	int normal_count = 0;
	int face_count = 0;
	int mtl_lib_count = 0;

	obj_count_entries(obj, &vertex_count, &tex_coords_count, &normal_count, &face_count, &mtl_lib_count);

	printf("Parsed obj file \"%s\":\n", MODEL_PATH);
	printf("\tVertices: %d\n", vertex_count);
	printf("\tTexture coordinates: %d\n", tex_coords_count);
	printf("\tNormals: %d\n", normal_count);
	printf("\tFaces: %d\n", face_count);
	printf("\tMaterial libraries: %d\n", mtl_lib_count);

	// Rewind the file pointer:
	rewind(obj);

	// Allocate memory from the counters:
	obj_vertex_entry_t* vertices = (obj_vertex_entry_t*) malloc(vertex_count * sizeof(obj_vertex_entry_t));
	check_error(vertices != NULL, "Failed to allocate memory for vertices.");

	obj_normal_entry_t* normals = (obj_normal_entry_t*) malloc(normal_count * sizeof(obj_normal_entry_t));
	check_error(normals != NULL, "Failed to allocate memory for normals.");

	obj_tex_coords_entry_t* tex_coords = (obj_tex_coords_entry_t*) malloc(tex_coords_count * sizeof(obj_tex_coords_entry_t));
	check_error(tex_coords != NULL, "Failed to allocate memory for texture coordinates.");

	// Allocate the vertex data array:
	user_data->vertex_data_count = 3 * face_count;

	vertex_data_t* vertex_data = (vertex_data_t*) malloc(user_data->vertex_data_count * sizeof(vertex_data_t));
	check_error(vertex_data != NULL, "Failed to allocate memory for vertex data.");

	// Walk all the entries in the obj file using a while loop.

	obj_entry_type_t entry_type;
	obj_entry_t entry;

	int vertex_counter = 0;
	int normal_counter = 0;
	int tex_coords_counter = 0;
	int vertex_data_counter = 0;

	while ((entry_type = obj_get_next_entry(obj, &entry)) != OBJ_ENTRY_TYPE_END)
	{
		switch (entry_type)
		{
		case OBJ_ENTRY_TYPE_VERTEX: vertices[vertex_counter++] = entry.vertex_entry; break;
		case OBJ_ENTRY_TYPE_NORMAL: normals[normal_counter++] = entry.normal_entry; break;
		case OBJ_ENTRY_TYPE_TEX_COORDS: tex_coords[tex_coords_counter++] = entry.tex_coords_entry; break;
		case OBJ_ENTRY_TYPE_FACE:
		{
			// Iterate over the three vertices of the face:
			const obj_face_index_triple_t* triples = entry.face_entry.triples;

			for (int i = 0; i < 3; i++)
			{
				// Use the triple indices to find the vertex / normal / tex coord entries:
				const obj_vertex_entry_t* vertex_entry = &vertices[triples[i].vertex_index];
				const obj_normal_entry_t* normal_entry = &normals[triples[i].normal_index];
				const obj_tex_coords_entry_t* tex_coords_entry = &tex_coords[triples[i].tex_coords_index];

				// Build the vertex data struct:
				vertex_data_t* new_vertex_data = &vertex_data[vertex_data_counter++];

				new_vertex_data->position[0] = (GLfloat)vertex_entry->x;
				new_vertex_data->position[1] = (GLfloat)vertex_entry->y;
				new_vertex_data->position[2] = (GLfloat)vertex_entry->z;

				new_vertex_data->normal[0] = (GLfloat)normal_entry->x;
				new_vertex_data->normal[1] = (GLfloat)normal_entry->y;
				new_vertex_data->normal[2] = (GLfloat)normal_entry->z;

				new_vertex_data->tex_coords[0] = (GLfloat)tex_coords_entry->u;
				new_vertex_data->tex_coords[1] = (GLfloat)tex_coords_entry->v;

				new_vertex_data->color[0] = 0x80;
				new_vertex_data->color[1] = 0x80;
				new_vertex_data->color[2] = 0x80;
			}

			break;
		}

		default: break;
		}
	}

	// Upload the data to the GPU:
	glBufferData(GL_ARRAY_BUFFER, user_data->vertex_data_count * sizeof(vertex_data_t), (const GLvoid*)vertex_data, GL_STATIC_DRAW);
	gl_check_error("glBufferData");

	// Release allocated memory and close the obj file:
	free(vertices);
	free(normals);
	free(tex_coords);
	free(vertex_data);

	fclose(obj);

	// Position attribute:
	// Number of attribute, number of components, data type, normalize, stride, pointer (= offset)
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, position));
	gl_check_error("glVertexAttribPointer [position]");

	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, color));
	gl_check_error("glVertexAttribPointer [color]");

	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, normal));
	gl_check_error("glVertexAttribPointer [normal]");

	glVertexAttribPointer(ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, tex_coords));
	gl_check_error("glVertexAttribPointer [tex_coords]");

	// Enable the attributes:
	glEnableVertexAttribArray(ATTRIB_POSITION);
	gl_check_error("glEnableVertexAttribArray [position]");

	glEnableVertexAttribArray(ATTRIB_COLOR);
	gl_check_error("glEnableVertexAttribArray [color]");

	glEnableVertexAttribArray(ATTRIB_NORMAL);
	gl_check_error("glEnableVertexAttribArray [normal]");

	glEnableVertexAttribArray(ATTRIB_TEX_COORDS);
	gl_check_error("glEnableVertexAttribArray [tex_coords]");

	// Store the VBO inside our user data:
	user_data->vbo = vbo;
}

void init_model(user_data_t* user_data)
{
	user_data->time = glfwGetTime();
	user_data->angle_y = 0;
	user_data->angle_x = 0;
	user_data->shading_case = 0;
}

void init_gl(GLFWwindow* window)
{
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	// Initialize our shader program:
	init_shader_program(user_data);

	// Initialize our texture:
	init_texture(user_data);

	// Initialize our uniforms:
	init_uniforms(user_data);

	// Initialize our vertex data:
	init_vertex_data(user_data);

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

	// Enable the depth test:
	glEnable(GL_DEPTH_TEST);
	gl_check_error("glEnable [GL_DEPTH_TEST]");

	// Init the Key Callback for the change of shading
	glfwSetKeyCallback(window, key_callback);
}

void update_gl(GLFWwindow* window)
{
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	// Update the time and calculate the delta:
	double new_time = glfwGetTime();
	double time_delta = new_time - user_data->time;

	user_data->time = new_time;

	// Adapt the angle using the time delta and the angular velocity:
	// user_data->angle_y = fmod(user_data->angle_y + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
	// user_data->angle_x = fmod(user_data->angle_x + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);

	int right_state = glfwGetKey(window, GLFW_KEY_RIGHT);
	if (right_state == GLFW_PRESS)
	{
		user_data->angle_y = fmod(user_data->angle_y + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
	}

	int left_state = glfwGetKey(window, GLFW_KEY_LEFT);
	if (left_state == GLFW_PRESS)
	{
		user_data->angle_y = fmod(user_data->angle_y + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
	}

	int up_state = glfwGetKey(window, GLFW_KEY_UP);
	if (up_state == GLFW_PRESS)
	{
		user_data->angle_x = fmod(user_data->angle_x + (-Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
	}

	int down_state = glfwGetKey(window, GLFW_KEY_DOWN);
	if (down_state == GLFW_PRESS)
	{
		user_data->angle_x = fmod(user_data->angle_x + (Y_ANGULAR_VELOCITY * time_delta), 2 * M_PI);
	}

	// Update the uniform:
	glUniform1f(user_data->angle_y_loc, user_data->angle_y);
	gl_check_error("glUniform1f [angle_y]");

	glUniform1f(user_data->angle_x_loc, user_data->angle_x);
	gl_check_error("glUniform1f [angle_x]");
	
	glUniform1ui(user_data->shading_case_loc, user_data->shading_case);
	gl_check_error("glUniform1ui [shading_case]");
}

void draw_gl(GLFWwindow* window)
{
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	// Clear the color buffer -> background color:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_check_error("glClear");

	// Draw our stuff!
	// Parameters: primitive type, start index, count
	glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count);
	gl_check_error("glDrawArrays");
}

void teardown_gl(GLFWwindow* window)
{
	printf("Tearing down ...\n");
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);

	// Delete the shader program:
	glDeleteProgram(user_data->shader_program);
	gl_check_error("glDeleteProgram");

	// Delete the VAO:
	glDeleteVertexArrays(1, &user_data->vao);
	gl_check_error("glDeleteVertexArrays");

	// Delete the VBO:
	glDeleteBuffers(1, &user_data->vbo);
	gl_check_error("glDeleteBuffers");
}
