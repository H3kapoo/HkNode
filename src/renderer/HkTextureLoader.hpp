#pragma once

#include <string>

namespace hkui
{
struct HkTextureInfo
{
    const char* texName; // name inside shader
    uint32_t texUnit{ 0 };
    uint32_t texId{ 0 };
    int width{ 0 }, height{ 0 }, nrChannels{ 0 };
};

class HkTextureLoader
{
public:
    static HkTextureInfo loadTexture(const std::string& filePath);
};
} // hkui