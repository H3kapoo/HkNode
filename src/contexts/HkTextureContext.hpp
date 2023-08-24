#pragma once

#include "../renderer/HkTexture.hpp"

namespace hkui
{
class HkTextureContext
{
public:
    HkTextureContext() = default;
    bool loadTexture(const std::string& loadPath);

    /* Getters */
    const HkTexture& getTexture() const;
private:
    HkTexture texture_;
};
} // hkui