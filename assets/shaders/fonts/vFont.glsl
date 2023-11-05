#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec3 color;
uniform mat4 model[400];
uniform mat4 proj;

out vec3 colorOut;
out vec2 texCoords;
flat out int index;

void main()
{
    colorOut = color;
    texCoords = aTexCoord;
    index = gl_InstanceID;
    gl_Position = proj * model[index] * vec4(aPos, 1.0); // maybe -1 at 3d?
}

// uniform vec3 color;
// uniform highp mat4 model;
// uniform highp mat4 proj;

// out vec3 colorOut;
// out vec2 texCoords;
// void main()
// {
//     colorOut = color;
//     texCoords = aTexCoord;
//     gl_Position = proj * model * vec4(aPos, 1.0); // maybe -1 at 3d?
// }
