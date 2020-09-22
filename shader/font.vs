#version 410
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = vertex;
    TexCoords = vertex.zw;
}  