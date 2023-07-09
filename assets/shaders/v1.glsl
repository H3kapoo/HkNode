#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 color;
uniform mat4 model;
uniform mat4 proj;

out vec3 colorOut;
void main()
{
    colorOut = color;
    gl_Position = proj * model * vec4(aPos.xyz, 1.0);
}
