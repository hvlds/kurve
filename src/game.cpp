#include "game.hpp"

#include <map>
#include <memory>
#include <vector>

#include "border_model.hpp"
#include "font.hpp"
#include "gl_calls.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "player_manager.hpp"
#include "player_model.hpp"
#include "point.hpp"
#include "shader.hpp"
#include "menu.hpp"
#include "side_panel.hpp"

Game::Game(GLFWwindow* window) {
    std::cout << "---- INIT GAME ----" << std::endl;
    this->window = window;

    this->has_players = false;

    this->font = std::make_shared<Font>();

    this->menu = std::make_shared<Menu>(window, this->font);
    this->side_panel = std::make_shared<SidePanel>(window, this->font);
    this->player_manager = std::make_shared<PlayerManager>(window);

    this->border_model = std::make_shared<BorderModel>();

    init_gl(this->window);
    glfwSetKeyCallback(window, key_callback);
}

void Game::loop() {
    while (!glfwWindowShouldClose(this->window)) {
        auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
        GameState game_state = user_data->game_state;
        if (game_state == GAME_MENU) {
            glClear(GL_COLOR_BUFFER_BIT);
            gl_check_error("glClear");
            this->menu->draw();
        } else if (
            game_state == GAME_ACTIVE 
            || game_state == GAME_PAUSE 
            || game_state == GAME_TRANSITION) {
            if (this->has_players == false) {
                this->player_manager->add_players();
                this->has_players = true;
            }

            // Update
            this->border_model->update(this->window);
            this->player_manager->update(this->window);

            // Clear the color buffer -> background color:
            glClear(GL_COLOR_BUFFER_BIT);
            gl_check_error("glClear");

            // Draw the models:
            this->border_model->draw();
            this->player_manager->draw();
            this->side_panel->draw(this->get_player_count());

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
    std::cout << "---- TERMINATE GAME ----" << std::endl;

    // Terminate the Border Model
    glDeleteProgram(this->border_model->shader_id);
    gl_check_error("glDeleteProgram");
    this->border_model->mesh->terminate();

    // Terminate the PlayerManager
    this->player_manager->terminate();

    // Destroy the window:
    glfwDestroyWindow(this->window);

    // Terminate GLFW:
    glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);
    GameState game_state = user_data->game_state;

    if (action == GLFW_RELEASE) {
        return;
    }

    if (game_state == GAME_MENU) {
        if (key == GLFW_KEY_ENTER) {
            // GAME_MENU -> GAME_ACTIVE
            if (user_data->game_state == GAME_MENU) {
                int player_count = 0;
                for (auto player_info : *user_data->player_info) {
                    if (player_info.is_active) {
                        player_count++;
                    }
                }
                if (player_count >= 2) user_data->game_state = GAME_TRANSITION;
            }
        }

        // Confirmation keys: activate player
        for (auto player_info : *user_data->player_info) {
            if (player_info.is_active == false 
                && key == player_info.control.left_key) {
                user_data->player_info->at(player_info.id - 1).is_active = true;
            }

            if (player_info.is_active == true 
                && key == player_info.control.right_key) {
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
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
    int player_count = 0;
    for (auto player_info : *user_data->player_info) {
        if (player_info.is_active == true) {
            player_count++;
        }
    }

    return player_count;
}