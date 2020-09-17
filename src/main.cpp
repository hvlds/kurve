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

	Game game(window);
	game.loop();	

	display.terminate();

	return 0;
}
