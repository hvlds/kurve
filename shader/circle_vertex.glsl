#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

// Uniforms:
uniform float trans_y;
uniform float trans_x;
uniform float trans_vec_x[4];
uniform float trans_vec_y[4];
out vec4 f_color;

void main()
{
	// Distance from the camera
	vec4 trans2 = vec4(trans_vec_x[gl_InstanceID], trans_vec_y[gl_InstanceID], 0.0, 3) * 2;

	vec4 position = v_position;
	position.y += trans_y;
	position.x += trans_x;

	gl_Position = position + trans2;
	f_color = v_color;
}
