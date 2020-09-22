#include "game.hpp"
#include "shader.hpp"
#include "player_model.hpp"
#include "border_model.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "point.hpp"
#include "player_manager.hpp"
#include "gl_calls.hpp"

#include <vector>
#include <memory>
#include <map>

Game::Game(GLFWwindow* window) {
	std::cout << "---- INIT GAME ----" << std::endl;
    this->window = window;
	
	auto player_manager = std::make_shared<PlayerManager>();
	this->player_manager = player_manager;

	Control control_1 = {
		GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN
	};
	std::array<GLubyte, 3> color_1 = {0xFF, 0x00, 0x00};
	this->player_manager->add_player(control_1, color_1);


	Control control_2 = {
		GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S
	};
	std::array<GLubyte, 3> color_2 = {0x00, 0xFF, 0x00};
	this->player_manager->add_player(control_2, color_2);

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
		this->player_manager->update(this->window);

		// Detect the collisions
		player_manager->detect_collisions();

		// Check how many players are still active
		auto active_players = player_manager->get_active_players();
		if (active_players.size() <= 1) {
			std::cout << "GAME OVER!" << std::endl;
			return;
 		}

		// Clear the color buffer -> background color:
		glClear(GL_COLOR_BUFFER_BIT);
		gl_check_error("glClear");

		// Draw the models:
		for (auto model : models) {
			model->draw();
		}
		this->player_manager->draw();

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