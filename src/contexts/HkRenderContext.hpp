#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "../utils/HkTextureLoader.hpp"

namespace hkui
{

class HkRenderContext //TODO: Rename to HkRenderConfig?
{
public:
    HkRenderContext() = default;

    uint32_t vaoId;
    uint32_t shaderId;
    std::vector<HkTextureInfo> texInfos;
    glm::mat4 windowProjMatrix;
    bool colorUniformEn{ false };
};
} // hkui