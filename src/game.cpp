#include "game.hpp"

#include <memory>

#include "border_model.hpp"
#include "font.hpp"
#include "gl_calls.hpp"
#include "menu.hpp"
#include "player_manager.hpp"
#include "side_panel.hpp"


Game::Game(GLFWwindow* window) {
    std::cout << "---- INIT Game ----" << std::endl;

    this->window = window;
    this->has_players = false;

    this->font = std::make_shared<Font>();
    this->menu = std::make_shared<Menu>(window, this->font);
    this->side_panel = std::make_shared<SidePanel>(window, this->font);
    this->player_manager = std::make_shared<PlayerManager>(window);
    this->border_model = std::make_shared<BorderModel>();
    this->game_over = std::make_shared<GameOver>(window, this->font);

    init_gl(this->window);
    glfwSetKeyCallback(window, key_callback);
}

void Game::loop() {
    while (!glfwWindowShouldClose(this->window)) {
        auto user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);
        GameState game_state = user_data->game_state;
        if (game_state == GAME_MENU) {
            glClear(GL_COLOR_BUFFER_BIT);
            gl_check_error("glClear");
            this->has_players = false;
            this->menu->draw();
        } else if (game_state == GAME_OVER) {
            glClear(GL_COLOR_BUFFER_BIT);
            gl_check_error("glClear");
            this->game_over->draw();
        } else if (game_state == GAME_ESCAPE) {
            // Reset the players and go back to the menu
            this->show_win_frames = 0;
            user_data->game_state = GAME_MENU;
            this->player_manager->terminate();
            reset_player_info(this->window);
        } else if (
            game_state == GAME_ACTIVE
            || game_state == GAME_WIN 
            || game_state == GAME_PAUSE 
            || game_state == GAME_TRANSITION) {

            // Generate the players in the first loop iteration
            if (this->has_players == false) {
                this->player_manager->add_players();
                this->has_players = true;
            }
            
            // Update only in the active frames
            if (game_state != GAME_WIN) {
                this->border_model->update(this->window);
                this->player_manager->update(this->window);
                player_manager->is_updated = false;
                player_manager->detect_collisions();                
            } else {
                player_manager->update_score();

                // Check if the actual score generates a GAME_OVER state
                GameState new_state = player_manager->check_score();
                if (new_state == GAME_OVER) {
                    user_data->game_state = new_state;
                    game_state = GAME_OVER;
                    this->game_over->get_winner();
                    this->player_manager->terminate();
                }
            }
            
            if (game_state != GAME_OVER) {
                // Clear the color buffer -> background color:
                glClear(GL_COLOR_BUFFER_BIT);
                gl_check_error("glClear");

                // Draw the models:
                this->border_model->draw();
                this->player_manager->draw();
                this->side_panel->draw(
                    this->get_player_count(),
                    this->player_manager->get_max_score());           

                // Check how many players are still active
                auto players_alive = player_manager->get_alive_players();
                auto players_dead = player_manager->get_dead_players();

                if (players_alive.size() <= 1 && players_dead.size() != 0) {
                    int id_winner;
                    if (players_alive.size() == 1) {
                        id_winner = players_alive.back();
                    } else {
                        id_winner = players_dead.back();
                    }
                    this->side_panel->set_winner(id_winner);
                    if (this->show_win_frames < 100) {
                        this->show_win_frames++;
                        user_data->game_state = GAME_WIN;
                    } else {
                        this->show_win_frames = 0;
                        player_manager->reset();
                        user_data->game_state = GAME_TRANSITION;
                    }
                }
            }
        }

        // Swap the buffers to avoid tearing:
        glfwSwapBuffers(this->window);

        // React to the window manager's messages (e.g. close button):
        glfwPollEvents();
    }
}

void Game::terminate() {
    std::cout << "---- TERMINATE Game ----" << std::endl;

    // Terminate the Border Model
    glDeleteProgram(this->border_model->shader_id);
    gl_check_error("glDeleteProgram");
    this->border_model->mesh->terminate();

    // Terminate the PlayerManager
    this->player_manager->terminate();
}

void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto user_data = (user_data_t*)glfwGetWindowUserPointer(window);

    if (action == GLFW_RELEASE) {
        return;
    }

    if (user_data->game_state == GAME_MENU) {
        if (key == GLFW_KEY_SPACE) {
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
                user_data->player_info->at(player_info.id - 1).is_active = 
                    true;
            }

            if (player_info.is_active == true 
                && key == player_info.control.right_key) {
                user_data->player_info->at(player_info.id - 1).is_active = 
                    false;
            }
        }
    } else {
        // Space can be different things depending of the context:
        // A confirmation key (go to next scene) or pause key (when active)
        if (key == GLFW_KEY_SPACE) {
            if (user_data->game_state == GAME_PAUSE) {
                user_data->game_state = GAME_ACTIVE;
            } else if (user_data->game_state == GAME_ACTIVE) {
                user_data->game_state = GAME_PAUSE;
            } else if (user_data->game_state == GAME_TRANSITION) {
                user_data->game_state = GAME_ACTIVE;
            } else if (user_data->game_state == GAME_OVER) {
                user_data->game_state = GAME_MENU;
                reset_player_info(window);
            }
        }
        // Go back to the menu only if the state is other than GAME_OVER
        if (key == GLFW_KEY_ESCAPE && user_data->game_state != GAME_OVER) {
            user_data->game_state = GAME_ESCAPE;
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