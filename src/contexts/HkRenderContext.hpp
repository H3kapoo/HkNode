#pragma once

#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../renderer/HkShader.hpp"

namespace hkui
{
/* Used to define render architecture, like quads, circles, triangles, etc */
struct HkRenderArch
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
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
    HkRenderContext();
    void setShaderSource(const std::string& vertSource, const std::string& fragSource);
    void render(const glm::mat4& projMat, const glm::mat4& modelMat);
    void setTextureRefId(const unsigned int textureId);
    void setupArch();

    /* Setters */
    void addTexture(const HkTextureInfo& texInfo);

    /* Getters */
    HkShader& getShader(); /* Non const, use it as a gateway */

public:
    // private:
    HkRenderArch renderArch;
    uint32_t vaoId;
    HkShader shader_;
    std::vector<HkTextureInfo> texInfos_;
    // shader
    // Texture
    // Pos
};
} // hkui