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
    void setShaderSource(const std::string& vertPath, const std::string& fragPath);
    void setInt(const char* location, int value);
    void setVec3f(const char* location, glm::vec3 value);
    void setMatrix4(const char* location, const glm::mat4 transform);

    /* Getters */
    uint32_t getShaderId() const;

private:
    int linkShaders(int vertShaderId, int fragShaderId);
    int compileShader(const std::string& sourcePath, int32_t shaderType);

    uint32_t shaderId_;

    /* Statics */
    static std::unordered_map<std::string, int32_t> shaderSourceToId_;
    static uint32_t currentlyActiveShaderId_;
};

} // hkui