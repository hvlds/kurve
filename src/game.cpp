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

	this->has_players = false;

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
		} else if(
			game_state == GAME_ACTIVE 
			|| game_state == GAME_PAUSE 
			|| game_state == GAME_TRANSITION) {
			
			if (this->has_players == false) {
				this->player_manager->add_players();
				this->has_players = true;
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
			
			// Show the right player names
			int player_count = this->get_player_count();
			float y_pos = 400.0f;
			float count = 0;

			for(auto player_info : *user_data->player_info) {
				std::string name = player_info.name;
				auto color = player_info.menu_color;
				if(player_info.is_active == true) {
					y_pos = y_pos - 50.0f ;
					auto score_text = std::to_string(player_info.score);
					this->font->draw_text(name, 450.0f, y_pos, 0.6f, color);
					this->font->draw_text(score_text, 400.0f, y_pos, 0.6f, color);
					count += 1.0f;
				}
			}

			user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(window);
			GameState game_state = user_data->game_state;
			if (game_state == GAME_PAUSE) {
				this->font->draw_text("Press SPACE", 400.0f, -300.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
				this->font->draw_text("to continue", 400.0f, -325.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
			} else if (game_state == GAME_ACTIVE) {
				this->font->draw_text("Press SPACE", 400.0f, -300.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
				this->font->draw_text("to pause", 400.0f, -325.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
			} else if (game_state == GAME_TRANSITION) {
				this->font->draw_text("Press SPACE", 400.0f, -300.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
				this->font->draw_text("to start", 400.0f, -325.0f, 0.7f, glm::vec3(1.0f, 1.0f, 1.0f));
			}

			// Detect the collisions
			player_manager->detect_collisions();

			// Check how many players are still active
			auto active_players = player_manager->get_alive_players();
			if (active_players.size() <= 1) {
				// std::cout << "GAME OVER!" << std::endl;
				player_manager->update_score();
				player_manager->reset();
				user_data->game_state = GAME_TRANSITION;
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
				int player_count = 0;
				for (auto player_info : *user_data->player_info) {
					if(player_info.is_active) {
						player_count++;
					}
				}
				if(player_count >= 2) user_data->game_state = GAME_TRANSITION;
			} 
		}
		
		// Confirmation keys: activate player
		for (auto player_info : *user_data->player_info) {
			if(player_info.is_active == false && key == player_info.control.left_key) {
				user_data->player_info->at(player_info.id - 1).is_active = true;
			}
			
			if(player_info.is_active == true && key == player_info.control.right_key) {
				user_data->player_info->at(player_info.id - 1).is_active = false;
			}
		}
	}

	if (key == GLFW_KEY_SPACE) {
		if (user_data->game_state == GAME_PAUSE) {
			user_data->game_state = GAME_ACTIVE;
		} else if (user_data->game_state == GAME_ACTIVE) {
			user_data->game_state = GAME_PAUSE;
		} else if (user_data->game_state == GAME_TRANSITION) {
			user_data->game_state = GAME_ACTIVE;
		}
	}
}

int Game::get_player_count() {
	user_data_t* user_data = (user_data_t*) glfwGetWindowUserPointer(this->window);
	int player_count = 0;
	for (auto player_info : *user_data->player_info) {
		if (player_info.is_active == true) {
			player_count++;
		}
	}
	
	return player_count;
}