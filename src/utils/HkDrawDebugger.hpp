#pragma once

#include <vector>
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
    void pushDraw10x10(const glm::ivec2 pos);

    void drawBuffer();

private:
    HkDrawDebugger() = default;
    void draw10x10(const glm::ivec2 pos);

    std::vector<glm::ivec2> buffer_;

    HkRenderContext rc_;
    HkTransformContext tc_;
};
} // hkui
