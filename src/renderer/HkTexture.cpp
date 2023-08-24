#include "HkTexture.hpp"

#include <iostream>

#include "../APIGate/GlfwGlewGate.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

namespace hkui
{
bool HkTexture::loadTexture(const std::string& filePath)
{
    unsigned char* data = stbi_load(filePath.c_str(), &width_, &height_, &nrChannels_, 0);
    if (!data)
    {
        std::cout << "Failed to load texture from: " << filePath << "\n";
        return false;
    }

    glGenTextures(1, &textureId_);
    glBindTexture(GL_TEXTURE_2D, textureId_);
    std::cout << "channels: " << nrChannels_ << "\n";
    std::cout << "height: " << height_ << "\n";
    std::cout << "width: " << width_ << "\n";

    // filters and repeat method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

int HkTexture::getWidth() const
{
    return width_;
}

int HkTexture::getHeight() const
{
    return height_;
}

int HkTexture::getNrChannels() const
{
    return nrChannels_;
}

unsigned int HkTexture::getTextureId() const
{
    return textureId_;
}
} // hkui