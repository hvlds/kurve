#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include "gl_calls.hpp"
#include "game.hpp"
#include "display.hpp"
#include "user.hpp"

int main(void)
{
	// Create our user data struct:
	user_data_t user_data =
	{
		.window_width = 800,
		.window_height = 600,
	};

	Display display(&user_data);
	GLFWwindow* window = display.get_window();

	std::cout << "Init game" << std::endl;
	Game game(window);

	std::cout << "Starting game loop" << std::endl;
	game.loop();	

	std::cout << "Terminating game" << std::endl;
	game.terminate();

	return 0;
}
