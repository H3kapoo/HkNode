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

    /* Setters */
    HkStyleContext& setColor(const glm::vec3 color);
    void setRowWrapping(const bool value);
    void setColWrapping(const bool value);

    void setUniforms(HkShader& shader);

    /* Getters */
    bool isRowWrappingEnabled();
    bool isColWrappingEnabled();

private:
    template<typename T>
    struct EnablerPair
    {
        bool enabled{ false };
        T value;
    };

    /* Shader enablers */
    EnablerPair<glm::vec3> colorEn;

    /* Attribs */
    bool rowWrap{ false };
    bool colWrap{ false };
};
} // hkui