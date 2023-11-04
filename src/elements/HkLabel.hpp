#pragma once

#include <string>
#include <memory>
#include <map>
#include <glm/glm.hpp>

#include "../base/HkNodeBase.hpp"
#include "../renderer/HkShader.hpp"
#include "../utils/HkFontLoader.hpp"

namespace hkui
{
class HkLabel : public HkNodeBase
{
public:
    HkLabel(const std::string& name);

    void setText(const std::string& text);

private:

    void onFirstHeartbeat();
    void postRenderAdditionalDetails();

    std::string text_;
    uint32_t vaoId_, vboId_, programId_;
    HkShader shader_;
    HkFontLoader fontLoader_;
    HkTextRenderConfig cfg_;
};

using HkLabelPtr = std::shared_ptr<HkLabel>;
using HkLabelCPtr = const std::shared_ptr<HkLabel>;
} // hkui
