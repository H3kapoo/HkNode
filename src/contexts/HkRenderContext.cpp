#include "HkRenderContext.hpp"

namespace hkui
{

void HkRenderContext::setConfig(const HkRenderConfig renderConfig)
{
    renderConfig_ = renderConfig;
}

void HkRenderContext::addTexture(const HkTextureInfo& texInfo)
{
    texInfos_.push_back(texInfo);
}

void HkRenderContext::setColorUniformEnabled(const bool value)
{
    colorUniformEnabled_ = value;
}


//TODO: This will no longer exist
void HkRenderContext::injectStyleContext(HkStyleContext* styleContext)
{
    styleContextInj_ = styleContext;
}
} // hkui
