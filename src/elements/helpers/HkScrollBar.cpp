#include "HkScrollBar.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name)
    : HkNodeBase(name, "ScrollBar")
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.2f, 1.0f, 0.2f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}
} // hkui