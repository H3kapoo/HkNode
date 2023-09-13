#include "HkStyleContext.hpp"

namespace hkui
{
HkStyleContext& HkStyleContext::setColor(const glm::vec3 color)
{
    colorEn.enabled = true;
    colorEn.value = color;
    return *this;
}

void HkStyleContext::setUniforms(HkShader& shader)
{
    if (colorEn.enabled)
        shader.setVec3f("color", colorEn.value);
}

bool HkStyleContext::isRowWrappingEnabled() { return rowWrap; }

bool HkStyleContext::isColWrappingEnabled() { return colWrap; }

} // hkui