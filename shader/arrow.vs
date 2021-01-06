#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float start_pos_y;
uniform float start_pos_x;

out vec4 f_color;

void main()
{	
	vec4 trans = vec4(start_pos_x, start_pos_y, 0.0, 20.0);
	vec4 position = v_position + trans;

	gl_Position = position;
	f_color = v_color;
}
