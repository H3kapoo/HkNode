#pragma once

#include <string>

namespace hkui
{
class HkTexture
{
public:
    HkTexture() : width_{ 0 }, height_{ 0 }, nrChannels_{ 0 }, textureId_{ 0 } {};
    bool loadTexture(const std::string& filePath);

    /*Getters*/
    int getWidth() const;
    int getHeight() const;
    int getNrChannels() const;
    unsigned int getTextureId() const;
private:
    int width_, height_, nrChannels_;
    unsigned int textureId_;

};
} // hkui