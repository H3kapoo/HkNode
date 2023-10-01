#include "HkTextureLoader.hpp"

#include <iostream>

#include "../APIGate/GlfwGlewGate.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

namespace hkui
{
HkTextureInfo HkTextureLoader::loadTexture(const std::string& filePath)
{
    HkTextureInfo texInfo;
    unsigned char* data = stbi_load(filePath.c_str(), &texInfo.width, &texInfo.height, &texInfo.nrChannels, 0);
    if (!data)
    {
        std::cerr << "Failed to load texture from: " << filePath << "\n";
        return texInfo;
    }

    glGenTextures(1, &texInfo.texId);
    glBindTexture(GL_TEXTURE_2D, texInfo.texId);
    std::cout << "channels: " << texInfo.nrChannels << "\n";
    std::cout << "height: " << texInfo.height << "\n";
    std::cout << "width: " << texInfo.width << "\n";

    // filters and repeat method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texInfo.width, texInfo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texInfo;
}
} // hkui