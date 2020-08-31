#version 410
layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 tex_coords;

out vec4 f_color;
out vec2 f_tex_coords;
out vec3 f_vertex;
out vec3 f_normal;

// Uniforms:
uniform float angle_y;
uniform float angle_x;
uniform uint shading_case;

void main()
{
	float near = 1.0;
	float far = 12.0;
	float left = -1.0;
	float right = 1.0;
	float top = 1.0;
	float bottom = -1.0;

	// mat4() works column-wise!
	mat4 frustum = mat4(
		2.0 * near / (right - left),		0.0,								0.0,								0.0,
		0.0,								2.0 * near / (top - bottom),		0.0,								0.0,
		(right + left) / (right - left), 	(top + bottom) / (top - bottom),	-(far + near) / (far - near),		-1.0,
		0.0,								0.0,								-2.0 * near * far / (far - near),	0.0
	);

	mat4 rot_y = mat4(
		cos(angle_y),		0.0,	-sin(angle_y),	0.0,
		0.0,				1.0,	0.0,			0.0,
		sin(angle_y),		0.0,	cos(angle_y),	0.0,
		0.0,				0.0,	0.0,			1.0
	);

	mat4 rot_x = mat4(
		1.0,		0.0,	  	    0.0,		    0.0,
		0.0,		cos(angle_x),	-sin(angle_x),	0.0,
		0.0,		sin(angle_x),	cos(angle_x),	0.0,
		0.0,		0.0,		    0.0,			1.0
	);

	vec4 trans = vec4(0.0, 0.0, -7.0, 1.0);
	vec4 pos = (rot_x * rot_y * v_position) + trans;

	if (shading_case == 0) { // Phong
		f_color = v_color;
		f_tex_coords = tex_coords;
		f_vertex = pos.xyz;
		f_normal = normalize((rot_x * rot_y * vec4(normal, 1.0)).xyz);

		gl_Position = frustum * pos;
	} else if (shading_case == 1) { // Depth
		f_color = v_color;
		f_tex_coords = tex_coords;
		f_vertex = pos.xyz;
		f_normal = normalize((rot_x * rot_y * vec4(normal, 1.0)).xyz);

		gl_Position = frustum * pos;
	} else { // Gouraud
		// Define the light position in eye space:
		vec3 light_pos = vec3(0.0, 0.0, 10.0);

		// Calculate the vector from the vertex to the light:
		vec3 light_vec = normalize(light_pos - pos.xyz);

		// Rotate and normalize the normal vector:
		vec3 n_normal = normalize((rot_x * rot_y * vec4(normal, 1.0)).xyz);

		// Calculate the dot product (depends on angle(normal, light)):
		float light_weight = max(dot(n_normal, light_vec), 0.1);

		gl_Position = frustum * vec4(pos.xyz, 1.0);
		f_color = light_weight * v_color;

		f_tex_coords = tex_coords;
		f_vertex = pos.xyz;
		f_normal = normalize((rot_x * rot_y * vec4(normal, 1.0)).xyz);

		gl_Position = frustum * pos;
	}
}
