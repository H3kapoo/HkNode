#pragma once

#include <memory>
#include <string>

#include "../base/HkNodeBase.hpp"
#include "../contexts/HkTextureContext.hpp"

namespace hkui
{
class HkImageView : public HkNodeBase
{
public:
    HkImageView(const std::string& name);

    void loadImage(const std::string& loadPath);
    void setSize(const glm::vec2& size);

private:
    HkTextureContext textureContext_;
};
using HkImageViewPtr = std::shared_ptr<HkImageView>;
using HkImageViewCPtr = const std::shared_ptr<HkImageView>;
} // hkui