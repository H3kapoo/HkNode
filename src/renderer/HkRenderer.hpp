#pragma once

#include <string>
#include <vector>

#include "../contexts/HkStyleContext.hpp"
#include "../contexts/HkRenderContext.hpp"
#include "HkShader.hpp"
#include "HkTextureLoader.hpp"

namespace hkui
{
enum class HkVertexArrayType
{
    QUAD,
};

struct HkRenderArch
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

class HkRenderer
{
public:
    int32_t addShaderSourceToCache(const std::string& vertSource, const std::string& fragSource);
    HkTextureInfo addTextureSourceToCache(const std::string& textureSource);
    int32_t addVertexArrayDataToCache(const HkVertexArrayType archType);
    void render(const HkRenderContext& renderConfig, const HkStyleContext& styleConfig, const glm::mat4& modelMat);

private:
    int32_t setupQuadArch();

    std::unordered_map<std::string, uint32_t> pathToShaderIdMap_;
    std::unordered_map<std::string, HkTextureInfo> pathToTextureInfoMap_;
    std::unordered_map<HkVertexArrayType, uint32_t> archNameToVaoIdMap_;
    uint32_t boundVaoId_{ 0 };
    uint32_t boundShaderId_{ 0 };

    HkShader shader_;
    static HkRenderArch renderArch_;
};
} // hkui