#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;

out vec4 f_color;

void main()
{	
	vec4 trans = vec4(0, 0, 0.0, 1.0);

	gl_Position = v_position;
	f_color = v_color;
}
