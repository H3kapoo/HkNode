#pragma once

#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../contexts/HkStyleContext.hpp"
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
    HkRenderContext() = default;
    void setShaderSource(const std::string& vertSource, const std::string& fragSource);
    void render(const glm::mat4& projMat, const glm::mat4& modelMat);
    void setTextureRefId(const unsigned int textureId);
    void setupArch();

    /* Setters */
    void addTexture(const HkTextureInfo& texInfo);

    void setColorUniformEnabled(const bool value);

    /* Getters */
    HkShader& getShader(); /* Non const, use it as a gateway */

    /* Injects */
    void injectStyleContext(HkStyleContext* styleContext);

private:
    static HkRenderArch renderArch;
    uint32_t vaoId; /* Note: only the first element created with this renderArch will have vao variable filled. THe rest of the object will have garbage
                                   Maybe in the future we could abstract this class even more.
                                   Let it slide for now (overflow and garbage val) but I shall come back later for it
    */
    HkShader shader_;
    std::vector<HkTextureInfo> texInfos_;

    /* Injects */
    HkStyleContext* styleContextInj_;

    /* Element enabled shader uniforms */
    bool colorUniformEnabled_{ false };

    /* Statics */
    static bool archCreated_;
    static uint32_t boundVaoId_;
    static uint32_t vaoId_;

    // shader
    // Texture
    // Pos
};
} // hkui