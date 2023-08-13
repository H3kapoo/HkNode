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
    case HkEvent::MouseMove: resolveMouseMovementEvent(); break;
    case HkEvent::MouseClick: resolveMouseClickEvent(); break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    auto& children = treeStruct_.getChildren();
    resolveConstraints(children);

    /* Render just the WF's container bounding box */
    const auto parent = treeStruct_.getParent();
    if (parent && parent->getType() == "RootWindowFrame")
    {
        glEnable(GL_SCISSOR_TEST);
        const auto& tc = node_.transformContext;
        glScissor(
            tc.pos.x - 1,
            sceneDataRef_.windowHeight - tc.pos.y - tc.scale.y + 1,
            tc.scale.x,
            tc.scale.y);
    }

    /* Normal rendering */
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    for (const auto& child : children)
    {
        child->getPayload()->updateMySelf();
    }

    postChildrenRendered();

    /* Disable scissors after rendering if we are the WF's container */
    if (parent && parent->getType() == "RootWindowFrame")
    {
        glDisable(GL_SCISSOR_TEST);
    }
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
    /*Element is in focus only if mouse if clicked and the mouse pos is inside thr element.
      Mouse offsets also get computed so is dragging occurs later on focused object, object doesn't
      just snap to clicked mouse position */
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.clickedMouseButton == HkMouseButton::Left
        && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
    {
        sceneDataRef_.focusedId = treeStruct_.getId();
        sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.pos - sceneDataRef_.mousePos;
    }
}

void HkNodeBase::resolveMouseClickEvent()
{
    /* Notify click on actually clicked object only*/
    if (sceneDataRef_.focusedId == treeStruct_.getId())
        onClick();
    /* Then notify the rest, notified already included */
    onGeneralMouseClick();
}

void HkNodeBase::resolveMouseMovementEvent()
{
    /* If scene detected a dragging action, separatelly specify that dragged element*/
    if (sceneDataRef_.isDragging && sceneDataRef_.focusedId == treeStruct_.getId())
        onDrag();

    /* And notify in general the rest of the tree objects. Dragged+Selected node will get a call from
       this function too.*/
    onGeneralMouseMove();
}

void HkNodeBase::postChildrenRendered() {}

void HkNodeBase::onDrag() {}
void HkNodeBase::onClick() {}
void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
} // hkui