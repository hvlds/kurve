#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float angle_y;
uniform float angle_x;

out vec4 f_color;

void main()
{
	gl_Position = v_position;
	f_color = v_color;
}
