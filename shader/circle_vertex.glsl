#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float angle_y;
uniform float angle_x;

out vec4 f_color;

void main()
{
	// Distance from the camera
	vec4 trans = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 position = v_position;
	position.y += angle_y;
	position.x += angle_x;

	gl_Position = position + trans;
	f_color = v_color;
}
