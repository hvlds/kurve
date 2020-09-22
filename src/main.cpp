#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include "gl_calls.hpp"
#include "game.hpp"
#include "display.hpp"
#include "user.hpp"

extern "C" {
#include "ft2build.h"
#include FT_FREETYPE_H  
} 

int main(void)
{
	// Create our user data struct:
	user_data_t user_data =
	{
		.window_width = 800,
		.window_height = 600,
	};

	// Testing FreeType
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "../fonts/UbuntuMono-R.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}

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
