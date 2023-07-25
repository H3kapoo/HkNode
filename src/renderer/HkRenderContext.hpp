#pragma once

#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "HkShader.hpp"

namespace hkui
{
class HkRenderContext
{
public:
    struct HkRenderArch
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
    };

    const HkRenderArch rectangleArch = {
        .vertices = {
            0.5f,  0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f},
        .indices = {
            0, 1, 3,
            1, 2, 3}
    };

    HkRenderContext();
    void setShaderSource(const std::string& vertSource, const std::string& fragSource);
    void render(const glm::mat4& projMat, const glm::mat4& modelMat);
    void setupArch();

private:
    HkRenderArch renderArch;
    uint32_t vaoId;
public:
    HkShader shader;
    // shader
    // Texture
    // Pos
};

} // hkui