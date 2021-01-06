#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float start_pos_y;
uniform float start_pos_x;
uniform float angle;

out vec4 f_color;

void main()
{	
	vec4 trans = vec4(start_pos_x, start_pos_y, 0.0, 20.0);
	vec4 position = v_position;

	mat4 rot_z = mat4(
		cos(angle),	-sin(angle), 0.0, 0.0,
		sin(angle),	cos(angle),	 0.0, 0.0,
		0.0,		0.0,	     1.0, 0.0,
		0.0,		0.0,	     0.0, 1.0
	);

	gl_Position = (rot_z * v_position) + trans;
	f_color = v_color;
}
