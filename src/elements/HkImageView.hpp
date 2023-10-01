#pragma once

#include <memory>
#include <string>

#include "../base/HkNodeBase.hpp"
#include "../renderer/HkTexture.hpp"

namespace hkui
{
class HkImageView : public HkNodeBase
{
public:
    HkImageView(const std::string& name);

    void loadImage(const std::string& loadPath);
    void setSize(const glm::vec2& size);

private:
    void onFirstHeartbeat() override;

    HkTexture texture_;
};
using HkImageViewPtr = std::shared_ptr<HkImageView>;
using HkImageViewCPtr = const std::shared_ptr<HkImageView>;
} // hkui