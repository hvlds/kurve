#include "game.hpp"
#include "shader.hpp"
#include "player_model.hpp"
#include "mesh.hpp"
#include "gl_calls.hpp"

#include <vector>

Game::Game(GLFWwindow* window) {
    this->window = window;
	user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

	PlayerModel player_1;

	this->models.push_back(&player_1);

	// Initialize everything related to OpenGL:
	init_gl(this->window);
}

void Game::loop() {
    while (!glfwWindowShouldClose(this->window))
	{
		// Update the models:
		std::cout << "Update models" << std::endl;
		for (auto model : models) {
			model->update(this->window);
		}

		std::cout << "Clear buffer bit" << std::endl;
		// Clear the color buffer -> background color:
		glClear(GL_COLOR_BUFFER_BIT);
		gl_check_error("glClear");

		// Draw the models:
		std::cout << "Draw models" << std::endl;
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

		model->mesh.terminate();
	}

	// Destroy the window:
	glfwDestroyWindow(this->window);

	// Terminate GLFW:
	glfwTerminate();
}