#ifndef DISPLAY_HPP
#define DISPLAY_HPP

extern "C" {
	// Include the GLAD loader *before* including GLFW!
	#include "glad/glad.h"
	
	// Include the GLFW library (should be the same for all OS):
	#include <GLFW/glfw3.h>
}
#include <iostream>
#include "gl_calls.hpp"
#include "user.hpp"

void error_callback(int error, const char* description);

/**
 * @brief Change of the screen ratio of the game. The function preserves
 * the ratio taking the shortest side of the window and centering in the
 * longest side
 * 
 * @param window 
 * @param fb_width 
 * @param fb_height 
 */
void framebuffer_size_callback(GLFWwindow* window, int fb_width, int fb_height);

/**
 * @brief Set new window size values in user_data_t after the window
 * changes its size
 *  
 * @param window 
 * @param width 
 * @param height 
 */
void window_size_callback(GLFWwindow* window, int width, int height);

class Display {
   private:
    GLFWwindow* window;
    user_data_t* user_data;
   public:
    Display(user_data_t* user_data);
    GLFWwindow* get_window();
    void terminate();
};

#endif