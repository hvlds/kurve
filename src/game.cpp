#include "game.hpp"
#include "shader.hpp"
#include "player_model.hpp"
#include "border_model.hpp"
#include "mesh.hpp"
#include "gl_calls.hpp"

#include <vector>
#include <memory>

Game::Game(GLFWwindow* window) {
    this->window = window;

	GLfloat x1, y1;
	x1 = 1.0;
	y1 = 1.0;
	std::array<GLubyte, 3> color_1 = {0xFF, 0x00, 0x00};
	auto player_1 = std::make_shared<PlayerModel>(x1, y1, color_1);
	player_1->set_keys(GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN);
	this->models.push_back(player_1);

	GLfloat x2, y2;
	x2 = -1.0;
	y2 = -1.0;
	std::array<GLubyte, 3> color_2 = {0x00, 0xFF, 0x00};
	auto player_2 = std::make_shared<PlayerModel>(x2, y2, color_2);
	player_2->set_keys(GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	this->models.push_back(player_2);

	auto border = std::make_shared<BorderModel>();
	this->models.push_back(border);

	init_gl(this->window);
}

void Game::loop() {
    while (!glfwWindowShouldClose(this->window))
	{
		// Update the models:
		for (auto model : models) {
			model->update(this->window);
		}

		// Clear the color buffer -> background color:
		glClear(GL_COLOR_BUFFER_BIT);
		gl_check_error("glClear");

		// Draw the models:
		for (auto model : models) {
			model->draw();
		}

		// Swap the buffers to avoid tearing:
		glfwSwapBuffers(this->window);

		// React to the window manager's messages (e.g. close button):
		glfwPollEvents();
	}
}

void Game::terminate() {
	for (auto model : models) {
		// Delete the shader program
		glDeleteProgram(model->shader_id);
    	gl_check_error("glDeleteProgram");

		model->mesh->terminate();
	}

	// Destroy the window:
	glfwDestroyWindow(this->window);

	// Terminate GLFW:
	glfwTerminate();
}