#include "HkNodeBase.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const std::string& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef())
    , hadFirstHeartbeat_{ false }
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

    //TODO: If children dont have children themselves, maybe no need to scissor?
    /* Compute renderable/inveractive area for each element */
    if (treeStruct_.getType() == "RootWindowFrame")
    {
        auto& tc = node_.transformContext;
        tc.setVPos(tc.getPos());
        tc.setVScale(tc.getScale());

        glScissor(
            tc.getPos().x - 1,
            sceneDataRef_.windowHeight - tc.getPos().y - tc.getScale().y + 1,
            tc.getScale().x,
            tc.getScale().y);
    }
    else if (parentTreeStruct && parentTreeStruct->getType() == "RootWindowFrame")
    {
        /* Minimize only container of windowframe */
        if (sceneDataRef_.isSceneMinimized && treeStruct_.getType() == "Container")
        {
            auto& tc = node_.transformContext;
            tc.setVPos({ 0,0 });
            tc.setVScale({ 0,0 });

            glScissor(0, 0, 0, 0);
        }
        else
        {
            auto& tc = node_.transformContext;
            tc.setVPos(tc.getPos());
            tc.setVScale(tc.getScale());

            glScissor(
                tc.getPos().x - 1,
                sceneDataRef_.windowHeight - tc.getPos().y - tc.getScale().y + 1,
                tc.getScale().x,
                tc.getScale().y);
        }
    }
    /* Basically use parent's visible area to bound the rendering of it's children */
    else if (parentTreeStruct && parentTreeStruct->getType() != "RootWindowFrame")
    {
        const auto& pTc = parentTreeStruct->getPayload()->node_.transformContext;

        HkTransformContext cx;
        cx.setPos(pTc.getVPos());
        cx.setScale(pTc.getVScale());

        /* Compute intersection TC with the parent Tc*/
        const auto bboxTc = node_.transformContext.computeBBoxWith(cx);
        node_.transformContext.setVPos(bboxTc.pos);
        node_.transformContext.setVScale(bboxTc.scale);

        glScissor(
            bboxTc.pos.x - 1,
            sceneDataRef_.windowHeight - bboxTc.pos.y - bboxTc.scale.y + 1,
            bboxTc.scale.x,
            bboxTc.scale.y);
    }

    /* Main HkEvents handler */
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
    case HkEvent::MouseScroll: resolveMouseScrollEvent(); break;
    case HkEvent::DropPath: break;
    }

    if (!hadFirstHeartbeat_)
    {
        onFirstHeartbeat();
        hadFirstHeartbeat_ = true;
    }

    /* Normal rendering */
    auto& tc = node_.transformContext;
    //TODO: Extend this performance gain further
    if (tc.getVScale().x && tc.getVScale().y)
    {
        node_.renderContext.render(sceneDataRef_.sceneProjMatrix, tc.getModelMatrix());
    }

    auto& children = treeStruct_.getChildren();
    /* Resolve child constraints relative to parent */
    resolveChildrenConstraints(children, {});

    /* Update children */
    for (uint32_t i = 0;i < children.size(); i++)
    {
        children[i]->getPayload()->updateMySelf();
    }

    /* Use this to render additional non interactive things if needed */
    /* Note: rescissoring to original parent is needed unfortunatelly */
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        tc.getVPos().x - 1,
        sceneDataRef_.windowHeight - tc.getVPos().y - tc.getVScale().y + 1,
        tc.getVScale().x,
        tc.getVScale().y);

    postRenderAdditionalDetails();

    // /* Disable scissors after rendering UI */
    glDisable(GL_SCISSOR_TEST);
}

/* Resolve constraints based on set policy on this node */
void HkNodeBase::resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children, const HkScrollbarsSize sbSizes)
{
    node_.constraintContext.resolveConstraints(children, sbSizes);
}

/* Try figure out if im the hovered one */
void HkNodeBase::resolveHover()
{
    // we need the last scrollable content weve met
    if (node_.transformContext.isPosInsideOfNodeViewableArea(sceneDataRef_.mousePos))
    {
        sceneDataRef_.hoveredId = treeStruct_.getId();
        resolveNearestActiveScrollbar();
    }
}

/* Try to figure out closest active scrollbar context near our hover position */
//TODO: Doesnt really do a great job but its fine for now. Refactor later
void HkNodeBase::resolveNearestActiveScrollbar()
{
    /* We shall ignore elements that overflow but that do not permit scrollbars. If we are a scrollbar, bingo. */
    if (treeStruct_.getType() == "ScrollBar" ||
        ((node_.constraintContext.isOverflowAllowedX_ || node_.constraintContext.isOverflowAllowedY_)
            && (node_.constraintContext.overflowXYSize_.x || node_.constraintContext.overflowXYSize_.y)))
    {
        sceneDataRef_.nearestScrollContainerId_ = treeStruct_.getId();
    }
}

/* Try figure out if im the focused one */
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

/* Resolve specific and general mouse click evt */
void HkNodeBase::resolveMouseClickEvent()
{
    /* Notify click on actually clicked object only*/
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId == treeStruct_.getId()) // maybe the hovered one? not the focused one?
        onClick();
    /* Notify release on actually released object only*/
    else if (!sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId == treeStruct_.getId())
        onRelease();
    /* Then notify the rest */
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId != treeStruct_.getId())
        onGeneralMouseClick();
}

/* Resolve specific and general mouse scroll evt */
void HkNodeBase::resolveMouseScrollEvent()
{
    /* Notify scroll on actually scrolled object only*/
    if (sceneDataRef_.hoveredId == treeStruct_.getId())
        onScroll();
    /* Then notify the rest */
    else
        onGeneralMouseScroll();
}

/* Resolve specific and general mouse mvmt evt */
void HkNodeBase::resolveMouseMovementEvent()
{
    //TODO: It works here but all shaders need to have hovered uniform
    // if (sceneDataRef_.hoveredId == treeStruct_.getId())
    // {
    //     node_.renderContext.getShader().setInt("hovered", 1);
    // }
    // else
    // {
    //     node_.renderContext.getShader().setInt("hovered", 0);
    // }

    /* If scene detected a dragging action, separatelly specify that dragged element*/
    if (sceneDataRef_.isDragging && sceneDataRef_.focusedId == treeStruct_.getId())
        onDrag();

    /* And notify in general the rest of the tree objects. Dragged+Selected node will get a call from
       this function too.*/
    onGeneralMouseMove();
}

/* Events to be consumed by derived if needed */
void HkNodeBase::postRenderAdditionalDetails() {}
void HkNodeBase::onFirstHeartbeat() {}
void HkNodeBase::onDrag() {}
void HkNodeBase::onClick() {}
void HkNodeBase::onRelease() {}
void HkNodeBase::onScroll() {}
void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
void HkNodeBase::onGeneralMouseScroll() {}

} // hkui