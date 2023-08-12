#include "HkDrawDebugger.hpp"

#include "../management/HkSceneManagement.hpp"

namespace hkui
{

HkDrawDebugger& HkDrawDebugger::get()
{
    static HkDrawDebugger instance;
    return instance;
}

void HkDrawDebugger::setDefaultDebugShader()
{
    rc_.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    rc_.shader.setVec3f("color", glm::vec3(0.941f, 0.494f, 0.812f));
}

void HkDrawDebugger::draw10x10(const glm::ivec2 pos)
{
    //TODO: Yeah, we should use depth test for these.. or always render them last
    //TODO: Yeah, we shall call this only once or on demand..
    setDefaultDebugShader();

    tc_.setPos(pos);
    tc_.setScale({ 10,10 });
    //TODO: yeah yeah coupling..its just a debugger, brick off
    rc_.render(HkSceneManagement::get().getSceneDataRef().sceneProjMatrix, tc_.getModelMatrix());
}
} // hkui