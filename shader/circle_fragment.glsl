
#version 410
precision mediump float;

in lowp vec4 f_color;
out vec4 out_color;

varying vec2 val;

void main()
{
	out_color = f_color;
}
