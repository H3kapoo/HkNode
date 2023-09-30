#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "HkRenderStore.hpp"

namespace hkui
{
//TODO: Maybe cache 'getUniformLocation'
class HkShader
{
public:
    HkShader() = default;
    void bind() const;
    void unbind() const;

    /* Setters */
    int32_t loadShaderFromSource(const std::string& vertPath, const std::string& fragPath);
    void setInt(const char* location, int value);
    void setVec3f(const char* location, glm::vec3 value);
    void setMatrix4(const char* location, const glm::mat4 transform);
    void setBackfedShaderId(uint32_t shaderId);
    void bindId(const uint32_t id);

    /* Getters */
    uint32_t getShaderId() const;

private:
    int linkShaders(int vertShaderId, int fragShaderId);
    int compileShader(const std::string& sourcePath, int32_t shaderType);

    uint32_t shaderId_;
    HkRenderStore* rsPtr;

    /* Statics */
    // static std::unordered_map<std::string, int32_t> shaderSourceToId_;
    // static uint32_t currentlyActiveShaderId_;
};

} // hkui