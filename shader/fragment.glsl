#version 410
precision mediump float;

in lowp vec4 f_color;
in vec2 f_tex_coords;
in vec3 f_vertex;
in vec3 f_normal;

out vec4 out_color;

uniform sampler2D tex;
uniform uint shading_case;

// Declare the functions
void depth();
void gouraud_flat();
void phong();

void depth() {
	out_color = texture(tex, f_tex_coords) * vec4(vec3(gl_FragCoord.z * gl_FragCoord.z), 1.0);
}

void gouraud_flat() {
	out_color = texture(tex, f_tex_coords) * f_color;
}

void phong() {
	// Position of the light source:
	vec3 light_pos = vec3(0.0, 0.0, 10.0);

	// Pre-calculate the vectors we need:
	vec3 light_dir = normalize(light_pos - f_vertex);
	vec3 reflect_dir = reflect(-light_dir, f_normal);
	vec3 view_dir = normalize(-f_vertex);

	// Ambient:
	vec3 i_amb = vec3(0.3);

	// Diffuse:
	vec3 i_diff = max(vec3(0.4) * dot(f_normal, light_dir), 0.1);

	// Specular:
	vec3 i_spec = vec3(1.0) * pow(max(dot(reflect_dir, view_dir), 0.0), 8);

	// Combine everything:
	out_color = texture(tex, f_tex_coords) * f_color * vec4(i_amb + i_diff + i_spec, 1.0);
}

void main()
{	
	if (shading_case == 0) {
		phong();
	} else if (shading_case == 1) {
		depth();
	} else {
		gouraud_flat();
	}
}
