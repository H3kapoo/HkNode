#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNode(name, "Button")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
{
    if (const auto& p = getParent().lock())
    {
        transformContext.setPos(p->transformContext.getPos());
    }


    transformContext.setOffsetFromParent({ 0,-30 });
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(1.0f, 0.0f, 0.0f)); // RED
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
}

//HkNode
void HkButton::updateMySelf()
{
    /*I need to reposition myself based on my parent*/
    if (const auto& p = getParent().lock())
    {
        transformContext.setPos(p->transformContext.getPos());
        transformContext.setOffsetFromParent({ 0,-30 });
    }

    /*main HkEvents handler*/
    switch (sceneDataRef.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize: break;
    case HkEvent::MouseMove:
        break;
    case HkEvent::MouseClick:
        if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
        {
            sceneDataRef.maybeSelectedNodeId = getId();
            sceneDataRef.mouseOffsetFromCenter = transformContext.getPos() - sceneDataRef.mousePos;
        }

        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }
    /*Don't forget to show node*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
}

void HkButton::setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

void HkButton::setSize(const glm::vec2& size) { transformContext.setScale(size); }

} // hkui
