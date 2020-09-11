#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "gl_calls.hpp"
#include "game.hpp"
#include "display.hpp"

int main(void)
{
	// Create our user data struct:
	user_data_t user_data =
	{
		.window_width = 800,
		.window_height = 600,
	};

	GLFWwindow* window = get_window(&user_data);

	Game game(window);
	game.loop();	

	teardown_gl(window);

	// Destroy the window:
	glfwDestroyWindow(window);

	// Terminate GLFW:
	glfwTerminate();

	return 0;
}
