#version 460 core

uniform sampler2D u_Texture;

out vec4 o_FragmentColor;

in vec2 TextureCoordinates;

void main()
{
    o_FragmentColor = vec4(texture(u_Texture, TextureCoordinates).rgb, 1);
}
