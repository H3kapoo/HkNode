#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace hkui
{
class HkShader
{
public:
    HkShader(const std::string& vertPath, const std::string& fragPath);
    void setShaderSource(const std::string& vertPath, const std::string& fragPath);
    void setInt(const char* location, int value);
    void setVec3f(const char* location, glm::vec3 value);
    void setMatrix4(const char* location, const glm::mat4 transform);
    void bind() const;
    void unbind() const;

private:
    int linkShaders(int vertShaderId, int fragShaderId);
    int compileShader(const std::string& sourcePath, int32_t shaderType);

    uint32_t shaderId;
};

} // hkui