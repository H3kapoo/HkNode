#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "../renderer/HkTextureLoader.hpp"

namespace hkui
{

class HkRenderContext
{
public:
    HkRenderContext() = default;

    uint32_t vaoId;
    uint32_t shaderId;
    std::vector<HkTextureInfo> texInfos;
    glm::mat4 windowProjMatrix;
    bool colorUniformEn{ false };
    bool customPostScissor{ false };
};
} // hkui