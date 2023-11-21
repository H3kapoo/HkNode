#version 330 core

out vec4 FragColor;

in vec4 colorOut;
in vec2 texCoords;
flat in int index;

uniform int letter[400];
uniform sampler2DArray letterTextures;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(letterTextures, vec3(texCoords, letter[index])).r);
    FragColor = vec4(colorOut) * sampled;
}
