#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace hkui
{

enum class HkVertexArrayType
{
    QUAD,
};

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

    uint32_t vaoId;
    uint32_t shaderId;
    std::vector<HkTextureInfo> texInfos;
    glm::mat4 windowProjMatrix;
    bool colorUniformEn{ false };
};
} // hkui