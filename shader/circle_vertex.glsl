#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float trans_y;
uniform float trans_x;
uniform mat4 trans;
out vec4 f_color;

void main()
{
	// Distance from the camera
	vec4 trans2 = vec4(gl_InstanceID, 0.0, 0.0, 0.0);

	vec4 position = v_position * trans;
	position.y += trans_y;
	position.x += trans_x;

	gl_Position = position + trans2;
	f_color = v_color;
}
