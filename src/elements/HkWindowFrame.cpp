#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNode(windowName, "RootFrame")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
    , elementsInitialized(false)
    , controlTopCont(std::make_shared<HkWFTopContainer>("WindowFrame_ControlTopCont"))
    , exitBtnNode(std::make_shared<HkButton>("ControlCont_ExitBtn"))
{
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.5f)); // BLUEish
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
}

// IHkRootNode
void HkWindowFrame::rootUpdateMySelf() { updateMySelf(); }

// HkNode
void HkWindowFrame::updateMySelf()
{
    /*main HkEvents handler*/
    switch (sceneDataRef.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize: break;
    case HkEvent::MouseMove:
        break;
    case HkEvent::MouseClick:
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    /*Don't forget to show node & update children*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    updateChildren();

    /*Render additional things AFTER children so that additional things appear on top of children*/
    renderAdditionalSelfElements();
}

void HkWindowFrame::renderAdditionalSelfElements()
{
    /* This is a side hack due to HkNode inheriting shared_from_this..we cannot do the shared_ptr creation inside
       the constructor and also the push.. */
    if (!elementsInitialized)
    {
        /* Set offset from center of window frame + scale control cont after window*/
        controlTopCont->transformContext.setScale({ transformContext.getScale().x, 30 });
        controlTopCont->transformContext.init(
            {
                transformContext.getPos().x,
                transformContext.getPos().y - transformContext.getScale().y / 2 - 15
            });

        elementsInitialized = true;
    }

    controlTopCont->updateMySelf();

    /*If parent moves, it will generate a XY vector representing direction of movement. Logic here is to move
      children in the same direction the same amount. To not move continously this amount, we need to clear the
      previously computed diff.*/
    transformContext.addPos(controlTopCont->transformContext.getDiff());
    controlTopCont->transformContext.clearDiff();
}

void HkWindowFrame::updateChildren()
{
    for (const auto& child : getChildren())
        child->updateMySelf();
}

void HkWindowFrame::pushChildren(const std::vector<HkNodePtr>& newChildren)
{
    HkNode::pushChildren(newChildren);
}

void HkWindowFrame::printTree() { HkNode::printTree(); }

void HkWindowFrame::setColor(const glm::vec3& color)
{
    renderContext.shader.setVec3f("color", color);
}

/* Here it is not necessary to use TC.init() for first initialization as diff is not used by anyone, so it can be left
to be as big as it want in the first frame */
void HkWindowFrame::setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

void HkWindowFrame::setSize(const glm::vec2& size) { transformContext.setScale(size); }

} // hkui
