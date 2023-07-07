#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace hkui
{
    class HkShader
    {
    public:
        HkShader(const std::string& vertPath, const std::string& fragPath)
        {
            linkShaders(compileShader(vertPath, GL_VERTEX_SHADER),
                compileShader(fragPath, GL_FRAGMENT_SHADER));
        }

        void bind() const { glUseProgram(shaderId); }
        void unbind() const { glUseProgram(0); }
    private:
        uint32_t shaderId;

        int linkShaders(int vertShaderId, int fragShaderId)
        {
            if (!vertShaderId || !fragShaderId)
                return -1;

            shaderId = glCreateProgram();

            glAttachShader(shaderId, vertShaderId);
            glAttachShader(shaderId, fragShaderId);
            glLinkProgram(shaderId);

            int success;
            char infoLog[512];
            glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

            if (!success)
            {
                glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
                std::cerr << "Could not link program "
                    << ": " << infoLog << std::endl;
                return -1;
            }

            glDeleteShader(vertShaderId);
            glDeleteShader(fragShaderId);

            return shaderId;
        }

        int compileShader(const std::string& sourcePath, int32_t shaderType)
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
    };
} // hkui