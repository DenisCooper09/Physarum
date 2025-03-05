#version 460 core

layout (location = 0) in vec3 i_Vertices;
layout (location = 1) in vec2 i_TextureCoordinates;

out vec2 TextureCoordinates;

void main()
{
    TextureCoordinates = i_TextureCoordinates;
    gl_Position = vec4(i_Vertices, 1);
}
