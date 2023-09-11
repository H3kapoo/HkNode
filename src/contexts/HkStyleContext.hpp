#pragma once

#include <vector>

#include <glm/glm.hpp>
#include "../renderer/HkShader.hpp"

namespace hkui
{

class HkStyleContext
{

public:
    HkStyleContext() = default;

    HkStyleContext& setColor(const glm::vec3 color)
    {
        colorPair.enabled = true;
        colorPair.value = color;
        return *this;
    }

    void setUniforms(HkShader& shader)
    {
        if (colorPair.enabled)
            shader.setVec3f("color", colorPair.value);
    }

private:
    template<typename T>
    struct EnablerPair
    {
        bool enabled{ false };
        T value;
    };

    /* Attribs */
    EnablerPair<glm::vec3> colorPair;
};
} // hkui