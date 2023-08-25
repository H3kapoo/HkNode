#version 330 core

out vec4 FragColor;

in vec3 colorOut;
flat in int hoveredOut;
flat in int focusedOut;

void main()
{
    // crude highligthing
   vec3 fColor = focusedOut == 1 ? vec3(0.9f) : vec3(1.0f);
   vec3 hColor = hoveredOut == 1 ? vec3(0.5f) : vec3(1.0f);
//    FragColor = hoveredOut == 1 ? vec4(colorOut*hfColor, 1.0f) : vec4(colorOut, 1.0f);
   FragColor = vec4(colorOut*hColor*fColor, 1.0f);
}
