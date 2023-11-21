#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec4 color;
uniform highp mat4 model;
uniform highp mat4 proj;

out vec4 colorOut;
void main()
{
    colorOut = color;
    gl_Position = proj * model * vec4(aPos.xyz, 1.0);
}
