#include "HkTextureContext.hpp"

namespace hkui
{
bool HkTextureContext::loadTexture(const std::string& loadPath)
{
    return texture_.loadTexture(loadPath);
}

const HkTexture& HkTextureContext::getTexture() const
{
    return texture_;
}
} // hkui