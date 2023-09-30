#pragma once

#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../contexts/HkStyleContext.hpp"
#include "../renderer/HkShader.hpp"
#include "../renderer/HkRenderStore.hpp"
#include "../renderer/HkRenderer.hpp"

namespace hkui
{

struct HkTextureInfo
{
    const char* texName;
    uint32_t texUnit{ 0 };
    uint32_t texId{ 0 };
};

class HkRenderContext
{
public:
    HkRenderContext() = default;
    void setTextureRefId(const unsigned int textureId);
    // void setupArch();

    /* Setters */
    void setConfig(const HkRenderConfig renderConfig);

    void addTexture(const HkTextureInfo& texInfo);

    void setColorUniformEnabled(const bool value);

    /* Getters */
    inline const HkRenderConfig& getConfig() const { return renderConfig_; }

    /* Injects */
    void injectStyleContext(HkStyleContext* styleContext);

    HkRenderConfig renderConfig_;
private:
    std::vector<HkTextureInfo> texInfos_;

    /* Injects */
    HkStyleContext* styleContextInj_;

    /* Element enabled shader uniforms */
    bool colorUniformEnabled_{ false };
};
} // hkui