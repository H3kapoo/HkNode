#version 330 core

out vec4 FragColor;

in vec3 colorOut;
in vec2 texCoords;

uniform sampler2D text;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
    FragColor = vec4(colorOut, 1.0f) * sampled;
}
