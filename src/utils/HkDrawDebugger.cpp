#include "HkDrawDebugger.hpp"

#include "../management/HkSceneManagement.hpp"

namespace hkui
{

HkDrawDebugger& HkDrawDebugger::get()
{
    static HkDrawDebugger instance;
    return instance;
}

void HkDrawDebugger::drawBuffer()
{
    setDefaultDebugShader();
    for (const auto el : buffer_)
    {
        draw10x10(el);
    }
    buffer_.clear();
}

//TODO: Bug: this fills buffer too quickly if function is called from the same place and there are more events from there in the same frame
void HkDrawDebugger::pushDraw10x10(const glm::ivec2 pos) { buffer_.push_back(pos); }

void HkDrawDebugger::setDefaultDebugShader()
{
    buffer_.reserve(8);
    // rc_.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    rc_.getShader().setVec3f("color", glm::vec3(1.0f, 1.0f, 1.0f));
}

void HkDrawDebugger::draw10x10(const glm::ivec2 pos)
{
    //TODO: Yeah, we should use depth test for these.. or always render them last
    //TODO: Yeah, we shall call this only once or on demand..

    tc_.setPos(pos);
    tc_.setScale({ 6,6 });
    //TODO: yeah yeah coupling..its just a debugger, brick off
    // rc_.render(HkSceneManagement::get().getSceneDataRef().sceneProjMatrix, tc_.getModelMatrix());
}
} // hkui