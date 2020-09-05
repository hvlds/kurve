#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float angle_y;
uniform float angle_x;

out vec4 f_color;

void main()
{
	vec4 position = v_position;
	position.y += angle_y;
	position.x += angle_x;
	gl_Position = position;
	f_color = v_color;
}
