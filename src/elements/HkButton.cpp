#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNodeBase(name, HkNodeType::Button)
    , textLabel_("{Internal}-TitleLabelFor " + name)
{
    // node_.styleContext.setColor(glm::vec4(0.761f, 0.267f, 0.78f, 1.0f));

    treeStruct_.pushChild(&textLabel_.treeStruct_);

    textLabel_.getStyle()
        .setColor({ 0, 0, 0, 0 })
        .setVHSizeConfig(
            { .type = HkSizeType::FitParent },
            { .type = HkSizeType::FitParent });
    // { .type = HkSizeType::PercParent, .value = 0.75 });

    textLabel_.setSelectTransparent(true);

    textLabel_.getTextStyle()
        .setFontColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .setTextVHAlign(HkTextUserConfig::HkTextVAlign::Center, HkTextUserConfig::HkTextHAlign::Center);
}

void HkButton::onFirstHeartbeat()
{
    std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;
    node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    textLabel_.injectWindowDataPtr(windowDataPtr_);
}

void HkButton::setText(const std::string& text) { textLabel_.setText(text); }
} // hkui
