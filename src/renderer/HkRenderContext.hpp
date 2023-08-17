#pragma once

#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "HkShader.hpp"

namespace hkui
{
/* Used to define render architecture, like quads, circles, triangles, etc */
struct HkRenderArch
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

class HkRenderContext
{
public:
    HkRenderContext();
    void setShaderSource(const std::string& vertSource, const std::string& fragSource);
    void render(const glm::mat4& projMat, const glm::mat4& modelMat);
    void setupArch();

    /* Getters */
    HkShader& getShader(); /* Non const, use it as a gateway */

private:
    HkRenderArch renderArch;
    uint32_t vaoId;
    HkShader shader_;
    // shader
    // Texture
    // Pos
};
} // hkui