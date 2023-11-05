#include "HkShader.hpp"

namespace hkui
{

int32_t HkShader::loadShaderFromSource(const std::string& vertPath, const std::string& fragPath)
{
    return linkShaders(compileShader(vertPath, GL_VERTEX_SHADER),
        compileShader(fragPath, GL_FRAGMENT_SHADER));
}

void HkShader::setInt(const char* location, int value)
{
    bind();
    int loc = glGetUniformLocation(shaderId_, location);
    if (loc == -1)
    {
        std::cerr << "Uniform '" << location << "' has not been found in bound shader!" << std::endl;
        return;
    }
    glUniform1i(loc, value);
}

void HkShader::setVec3f(const char* location, glm::vec3 value)
{
    bind();
    int loc = glGetUniformLocation(shaderId_, location);
    if (loc == -1)
    {
        std::cerr << "Uniform '" << location << "' has not been found in bound shader!" << std::endl;
        return;
    }
    glUniform3f(loc, value.x, value.y, value.z);
}

void HkShader::setMatrix4(const char* location, const glm::mat4 transform)
{
    bind();
    int transformLoc = glGetUniformLocation(shaderId_, location);
    if (transformLoc == -1)
    {
        std::cerr << "Uniform '" << location << "' has not been found in bound shader!" << std::endl;
        return;
    }
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
}

void HkShader::bind() const
{
    glUseProgram(shaderId_);
}

void HkShader::bindId(const uint32_t id)
{
    shaderId_ = id;
    bind();
}

void HkShader::unbind() const { glUseProgram(0); }

int HkShader::linkShaders(int vertShaderId, int fragShaderId)
{
    if (!vertShaderId || !fragShaderId)
        return -1;

    shaderId_ = glCreateProgram();

    glAttachShader(shaderId_, vertShaderId);
    glAttachShader(shaderId_, fragShaderId);
    glLinkProgram(shaderId_);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderId_, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderId_, 512, nullptr, infoLog);
        std::cerr << "Could not link program "
            << ": " << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return shaderId_;
}

int HkShader::compileShader(const std::string& sourcePath, int32_t shaderType)
{
    std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAG";
    std::ifstream shaderFile(sourcePath);
    if (!shaderFile)
    {
        std::cerr << "Could not open " << type << " shader file at: " << sourcePath << '\n';
        return -1;
    }

    std::stringstream stream;
    stream << shaderFile.rdbuf();
    std::string content = stream.str();
    const char* src = content.c_str();
    shaderFile.close();

    uint32_t shaderPart = glCreateShader(shaderType);
    glShaderSource(shaderPart, 1, &src, NULL);
    glCompileShader(shaderPart);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderPart, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderPart, 512, NULL, infoLog);
        std::cerr << "Compile failed for shader " << type << " :" << infoLog << '\n';
        return -1;
    }

    return shaderPart;
}

uint32_t HkShader::getShaderId() const
{
    return shaderId_;
}
} // hkui