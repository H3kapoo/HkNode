#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec4 color;
uniform mat4 model[400];
uniform mat4 proj;

out vec4 colorOut;
out vec2 texCoords;
flat out int index;

void main()
{
    colorOut = color;
    texCoords = aTexCoord;
    index = gl_InstanceID;
    gl_Position = proj * model[index] * vec4(aPos, 1.0); // maybe -1 at 3d?
}
