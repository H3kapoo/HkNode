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

void HkStyleContext::setRowWrapping(const bool value) { rowWrap = value; }

void HkStyleContext::setColWrapping(const bool value) { colWrap = value; }

bool HkStyleContext::isRowWrappingEnabled() { return rowWrap; }

bool HkStyleContext::isColWrappingEnabled() { return colWrap; }

} // hkui