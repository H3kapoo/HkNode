#pragma once

#include <string>
#include <unordered_map>

namespace hkui
{
struct HkRenderStore
{
    std::unordered_map<std::string, uint32_t> pathToShaderIdMap;
    std::unordered_map<std::string, uint32_t> archNameToVaoIdMap;
    uint32_t boundVaoId{ 0 };
    uint32_t boundShaderId{ 0 };
};
} // hkui