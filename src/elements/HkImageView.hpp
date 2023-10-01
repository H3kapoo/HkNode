#pragma once

#include <memory>
#include <string>

#include "../base/HkNodeBase.hpp"
// #include "../renderer/HkTextureLoader.hpp"

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

    bool earlyImgRequest_{ false };
    std::string earlyImgPath_;
};
using HkImageViewPtr = std::shared_ptr<HkImageView>;
using HkImageViewCPtr = const std::shared_ptr<HkImageView>;
} // hkui