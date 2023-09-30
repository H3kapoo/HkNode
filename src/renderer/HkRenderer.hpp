#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "../contexts/HkStyleContext.hpp"
#include "HkShader.hpp"

namespace hkui
{
struct HkRenderArch
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

struct HkRenderConfig
{
    uint32_t vaoId;
    uint32_t shaderId;
    std::vector<uint32_t> texIds;
    glm::mat4 windowProjMatrix;
    bool colorUniformEn{ false };
};

class HkRenderer
{
public:
    int32_t addShaderSourceToCache(const std::string& vertSource, const std::string& fragSource);
    int32_t addVertexArrayDataToCache(const std::string& type); //TODO: Shall be enum
    void render(const HkRenderConfig& renderConfig, const HkStyleContext& styleConfig, const glm::mat4& modelMat);

private:
    int32_t setupQuadArch();

    std::unordered_map<std::string, uint32_t> pathToShaderIdMap_;
    std::unordered_map<std::string, uint32_t> archNameToVaoIdMap_;
    uint32_t boundVaoId_{ 0 };
    uint32_t boundShaderId_{ 0 };

    HkShader shader_;
    static HkRenderArch renderArch_;
};
} // hkui