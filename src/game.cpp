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
	
	auto player_manager = std::make_shared<PlayerManager>(window);
	this->player_manager = player_manager;

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
			glClear(GL_COLOR_BUFFER_BIT);
			gl_check_error("glClear");	
			this->menu->draw();
		} else if(game_state == GAME_ACTIVE || game_state == GAME_PAUSE) {
			
			if (this->count == 0) {
				this->player_manager->add_players();
				this->count++;
			}

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
			this->font->draw_text("Slytherin", 400.0f, 350.0f, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

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
				// std::cout << "GAME OVER!" << std::endl;
				user_data->game_state = GAME_PAUSE;
				// return;
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
	GameState game_state = user_data->game_state; 
	
	if(action == GLFW_RELEASE) {
		return;	
	}

	if(game_state == GAME_MENU) {
		if (key == GLFW_KEY_ENTER) {
			// GAME_MENU -> GAME_ACTIVE
			if (user_data->game_state == GAME_MENU) {
				user_data->game_state = GAME_ACTIVE;
			} 
		}
		// Links: L.Ctrl - 1 - M - L.Arrow - O - B
		
		// Confirmation keys: activate player
		if (user_data->is_player_1_active == false && key == GLFW_KEY_LEFT_CONTROL) {
			user_data->is_player_1_active = true;
		} 
		if (user_data->is_player_2_active == false && key == GLFW_KEY_1) {
			user_data->is_player_2_active = true;
		}
		if (user_data->is_player_3_active == false && key == GLFW_KEY_M) {
			user_data->is_player_3_active = true;
		}
		if (user_data->is_player_4_active == false && key == GLFW_KEY_LEFT) {
			user_data->is_player_4_active = true;
		}
		if (user_data->is_player_5_active == false && key == GLFW_KEY_O) {
			user_data->is_player_5_active = true;
		}
		if (user_data->is_player_6_active == false && key == GLFW_KEY_B) {
			user_data->is_player_6_active = true;
		}
	
		// Cancellation keys: deactivate player
		if (user_data->is_player_1_active == true && key == GLFW_KEY_LEFT_ALT) {
			user_data->is_player_1_active = false;
		}
		
		if (user_data->is_player_2_active == true && key == GLFW_KEY_Q) {
			user_data->is_player_2_active = false;
		}
		if (user_data->is_player_3_active == true && key == GLFW_KEY_COMMA) {
			user_data->is_player_3_active = false;
		}
		
		if (user_data->is_player_4_active == true && key == GLFW_KEY_RIGHT) {
			user_data->is_player_4_active = false;
		}
		
		if (user_data->is_player_5_active == true && key == GLFW_KEY_P) {
			user_data->is_player_5_active = false;
		}
		
		if (user_data->is_player_6_active == true && key == GLFW_KEY_N) {
			user_data->is_player_6_active = false;
		}		

	}

	if (key == GLFW_KEY_SPACE) {
		if (user_data->game_state == GAME_PAUSE) {
			user_data->game_state = GAME_ACTIVE;
		} else if (user_data->game_state == GAME_ACTIVE) {
			user_data->game_state = GAME_PAUSE;
		}
	}
}