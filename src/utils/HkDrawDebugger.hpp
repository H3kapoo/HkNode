#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../APIGate/GlfwGlewGate.hpp"

#include "../renderer/HkRenderContext.hpp"
#include "../renderer/HkTransformContext.hpp"

namespace hkui
{
class HkDrawDebugger
{
public:
    HkDrawDebugger(const HkDrawDebugger&) = delete;
    void operator=(const HkDrawDebugger&) = delete;

    static HkDrawDebugger& get();

    void setDefaultDebugShader();
    void draw10x10(const glm::ivec2 pos);

private:
    HkDrawDebugger() = default;

    HkRenderContext rc_;
    HkTransformContext tc_;
};
} // hkui
