#include "HkNodeBase.hpp"

#include "../utils/HkDrawDebugger.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const std::string& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef())
{
    node_.constraintContext.setRootTc(&node_.transformContext);
}

/*
    Handles any event sent by the SM to the node, redering, and scissoring.
    Shall not be implemented by the child unless you really have no other choice.
*/
void HkNodeBase::updateMySelf()
{
    const auto& parentTreeStruct = treeStruct_.getParent();
    glEnable(GL_SCISSOR_TEST);

    /* Compute renderable/inveractive area for each element */
    if ((treeStruct_.getType() == "RootWindowFrame")
        || (parentTreeStruct && parentTreeStruct->getType() == "RootWindowFrame"))
    {
        auto& tc = node_.transformContext;
        tc.setVPos(tc.getPos());
        tc.setVScale(tc.getScale());

        // HkDrawDebugger::get().pushDraw10x10({ tc.getVPos().x, tc.getVPos().y });
        // HkDrawDebugger::get().pushDraw10x10({ tc.getVPos().x + tc.getVScale().x, tc.getVPos().y + tc.getVScale().y });

        glScissor(
            tc.getPos().x - 1,
            sceneDataRef_.windowHeight - tc.getPos().y - tc.getScale().y + 1,
            tc.getScale().x,
            tc.getScale().y);
    }
    /* Basically use parent's visible area to bound the rendering of it's children */
    else if (parentTreeStruct && parentTreeStruct->getType() != "RootWindowFrame")
    {
        // glEnable(GL_SCISSOR_TEST);
        const auto& pTc = parentTreeStruct->getPayload()->node_.transformContext;

        HkTransformContext cx;
        cx.setPos(pTc.getVPos());
        cx.setScale(pTc.getVScale());

        /* Compute intersection TC with the parent Tc*/
        const auto bboxTc = node_.transformContext.computeBBoxWith(cx);
        node_.transformContext.setVPos(bboxTc.pos);
        node_.transformContext.setVScale(bboxTc.scale);

        // HkDrawDebugger::get().pushDraw10x10({ bboxTc.pos.x, bboxTc.pos.y });
        // HkDrawDebugger::get().pushDraw10x10({ bboxTc.pos.x + bboxTc.scale.x, bboxTc.pos.y + bboxTc.scale.y });
        glScissor(
            bboxTc.pos.x - 1,
            sceneDataRef_.windowHeight - bboxTc.pos.y - bboxTc.scale.y + 1,
            bboxTc.scale.x,
            bboxTc.scale.y);
    }

    /*Main HkEvents handler */
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

    //TODO: For later: maybe just the generalUpdate event should render on the screen
    // and the other ones not

    /* Normal rendering */
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    auto& children = treeStruct_.getChildren();
    /* Resolve child constraints relative to parent */
    resolveConstraints(children);

    /* Update children */
    for (const auto& child : children)
    {
        child->getPayload()->updateMySelf();
    }

    /* Use this to render additional non interactive things if needed */
    /* Note: rescissoring to original parent is needed unfortunatelly */
    const auto& tc = node_.transformContext;
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        tc.getVPos().x - 1,
        sceneDataRef_.windowHeight - tc.getVPos().y - tc.getVScale().y + 1,
        tc.getVScale().x,
        tc.getVScale().y);

    postRenderAdditionalDetails();

    /* Disable scissors after rendering UI */
    glDisable(GL_SCISSOR_TEST);
}

void HkNodeBase::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    node_.constraintContext.resolveConstraints(children);
}

void HkNodeBase::resolveHover()
{
    if (node_.transformContext.isPosInsideOfNodeViewableArea(sceneDataRef_.mousePos))
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
        && node_.transformContext.isPosInsideOfNodeViewableArea(sceneDataRef_.mousePos))
    {
        sceneDataRef_.focusedId = treeStruct_.getId();
        sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.getPos() - sceneDataRef_.mousePos;
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

void HkNodeBase::postRenderAdditionalDetails() {}

void HkNodeBase::onDrag() {}
void HkNodeBase::onClick() {}
void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
} // hkui