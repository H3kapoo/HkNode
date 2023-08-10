#include "HkNodeBase.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const std::string& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef())
{
    node_.constraintContext.setRootTc(&node_.transformContext);
}

void HkNodeBase::updateMySelf()
{
    /*main HkEvents handler*/
    switch (sceneDataRef_.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::HoverScan: resolveHover(); break;
    case HkEvent::FocusScan: resolveFocus(); break;
    case HkEvent::GeneralUpdate: onGeneralUpdate(); break;
    case HkEvent::WindowResize: onWindowResize(); break;
    case HkEvent::MouseMove: onGeneralMouseMove(); break;
    case HkEvent::MouseClick: onGeneralMouseClick(); break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    auto& children = treeStruct_.getChildren();
    resolveConstraints(children);

    /* Dont render out of container bounds items (pixels really). Use paren't bounding box to determine if
       the child should be rendered. Use those bounds to create scissor box. However due to the nature of
       RootWindowFrame that defines a children outside of it's bounding box, scissor test for it shall be skipped*/
    const auto parent = treeStruct_.getParent();
    if (parent && parent->getType() != "RootWindowFrame")
    {
        // glEnable(GL_SCISSOR_TEST);
        const auto& pTc = parent->getPayload()->node_.transformContext;
        glScissor(
            pTc.pos.x - pTc.scale.x / 2,
            sceneDataRef_.windowHeight - pTc.pos.y - pTc.scale.y / 2,
            pTc.scale.x,
            pTc.scale.y);
    }

    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    for (const auto& child : children)
    {
        child->getPayload()->updateMySelf();
    }

    /* Disable scissors after rendering. Technically just one at end of frame just be enough but oh well */
    glDisable(GL_SCISSOR_TEST);
}

void HkNodeBase::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    node_.constraintContext.resolveConstraints(children);
}

void HkNodeBase::resolveHover()
{
    if (node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
    {
        sceneDataRef_.hoveredId = treeStruct_.getId();
    }
}

void HkNodeBase::resolveFocus()
{
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.clickedMouseButton == HkMouseButton::Left
        && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
    {
        sceneDataRef_.focusedId = treeStruct_.getId();
        sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.pos - sceneDataRef_.mousePos;
    }
}

void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
} // hkui