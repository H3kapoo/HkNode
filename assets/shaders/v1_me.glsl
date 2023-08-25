#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 color;
uniform mat4 model;
uniform mat4 proj;
uniform int hovered;
uniform int focused;

out vec3 colorOut;
flat out int hoveredOut;
flat out int focusedOut;

void main()
{
    colorOut = color;
    hoveredOut = hovered;
    focusedOut = focused;
    gl_Position = proj * model * vec4(aPos.xyz, 1.0);
}
