#include "game.hpp"
#include "gl_calls.hpp"

Game::Game(GLFWwindow* window) {
    this->window = window;
	
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