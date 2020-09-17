#include "game.hpp"
#include "shader.hpp"
#include "player_model.hpp"
#include "mesh.hpp"
#include "gl_calls.hpp"

Game::Game(GLFWwindow* window) {
    this->window = window;
	user_data_t* user_data = (user_data_t*)glfwGetWindowUserPointer(this->window);

	PlayerModel player_1(user_data);

	// Initialize everything related to OpenGL:
	init_gl(this->window);
}

int Game::loop() {
    while (!glfwWindowShouldClose(this->window))
	{
		// Update the model:
		update_gl(this->window);

		// Draw the next frame:
		draw_gl(this->window);

		// Swap the buffers to avoid tearing:
		glfwSwapBuffers(this->window);

		// React to the window manager's messages (e.g. close button):
		glfwPollEvents();
	}

    return 0;
}