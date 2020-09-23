#include "game.hpp"
#include "shader.hpp"
#include "player_model.hpp"
#include "border_model.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "point.hpp"
#include "player_manager.hpp"
#include "gl_calls.hpp"
#include "font.hpp"


#include <vector>
#include <memory>
#include <map>

Game::Game(GLFWwindow* window) {
	std::cout << "---- INIT GAME ----" << std::endl;
    this->window = window;

	auto menu = std::make_shared<Menu>(window);
	this->menu = menu;
	
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

	auto font = std::make_shared<Font>();
	this->font = font;

	init_gl(this->window);
	glfwSetKeyCallback(window, key_callback);
}

void Game::loop() {
    while (!glfwWindowShouldClose(this->window)) {
		user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);
		GameState game_state = user_data->game_state; 
		if(game_state == GAME_MENU) {
			this->menu->draw();
		} else if(game_state == GAME_ACTIVE || game_state == GAME_PAUSE) {		
			// Update the models:
			for (auto model : models) {
				model->update(this->window);
			}
			this->player_manager->update(this->window);

			// Clear the color buffer -> background color:
			glClear(GL_COLOR_BUFFER_BIT);
			gl_check_error("glClear");		

			// Draw the models:
			for (auto model : models) {
				model->draw();
			}
			this->player_manager->draw();

			this->font->draw_text("Gryffindor", 400.0f, 400.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			this->font->draw_text("Slytherin", 400.0f, 375.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

			user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);
			GameState game_state = user_data->game_state;
			if (game_state == GAME_PAUSE) {
				this->font->draw_text("Press SPACE", 400.0f, -300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
				this->font->draw_text("to continue", 400.0f, -325.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
			} else if (game_state == GAME_ACTIVE) {
				this->font->draw_text("Press SPACE", 400.0f, -300.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
				this->font->draw_text("to pause", 400.0f, -325.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
			}

			// Detect the collisions
			player_manager->detect_collisions();

			// Check how many players are still active
			auto active_players = player_manager->get_active_players();
			if (active_players.size() <= 1) {
				std::cout << "GAME OVER!" << std::endl;
				return;
			}
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {	
	user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(window);

	if(action == GLFW_RELEASE) {
		return;	
	}

	if (key == GLFW_KEY_SPACE) {
		if (user_data->game_state == GAME_PAUSE) {
			user_data->game_state = GAME_ACTIVE;
		} else if (user_data->game_state == GAME_ACTIVE) {
			user_data->game_state = GAME_PAUSE;
		}
	}

	if (key == GLFW_KEY_ENTER) {
		// GAME_MENU -> GAME_ACTIVE
		if (user_data->game_state == GAME_MENU) {
			user_data->game_state = GAME_ACTIVE;
		} 
	}
}