#include "HkNodeBase.hpp"

#include "../management/HkSceneManagement.hpp" //TODO: THis needs to disappear when no more static scene manager

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const HkNodeType& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef())
{
    node_.constraintContext.injectTransformContext(&node_.transformContext);
    node_.constraintContext.injectStyleContext(&node_.styleContext);
    node_.renderContext.injectStyleContext(&node_.styleContext);
    node_.renderContext.setColorUniformEnabled(true);
}

void HkNodeBase::renderMySelf()
{
    auto& tc = node_.transformContext;

    /* We only render the visible area of the UI element as calculated in the update pass*/
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        tc.getVPos().x - 1,
        sceneDataRef_.windowSize.y - tc.getVPos().y - tc.getVScale().y + 1,
        tc.getVScale().x,
        tc.getVScale().y);

    //TODO: tc.isAnyDifference() is correct here but main() loop doesnt know not to clear screen on each pass anymore
    // A flag maybe is needed? Conditional clear?
    /* Normal rendering */
    if (tc.getVScale().x && tc.getVScale().y)
    {
        node_.renderContext.render(sceneDataRef_.sceneProjMatrix, tc.getModelMatrix());

        /* Update children. Also don't require bellow children to be rendered if parent can't be rendered itself */
        auto& children = treeStruct_.getChildren();
        for (uint32_t i = 0;i < children.size(); i++)
        {
            children[i]->getPayload()->renderMySelf();
        }
    }

    /* Use this to render additional non interactive things if needed */
    /* Note: rescissoring to original parent is needed unfortunatelly */
    // glEnable(GL_SCISSOR_TEST);
    // glScissor(
    //     tc.getVPos().x - 1,
    //     sceneDataRef_.windowHeight - tc.getVPos().y - tc.getVScale().y + 1,
    //     tc.getVScale().x,
    //     tc.getVScale().y);

    // postRenderAdditionalDetails();

    /* Disable scissors after rendering UI */
    glDisable(GL_SCISSOR_TEST);
}

/*
    Handles any event sent by the SM to the node, redering, and scissoring.
    Shall not be implemented by the child unless you really have no other choice.
*/
void HkNodeBase::updateMySelf()
{
    const auto& parentTreeStruct = treeStruct_.getParent();
    auto& tc = node_.transformContext;

    /* Compute renderable/inveractive area for each element */
    if (treeStruct_.getType() == HkNodeType::RootWindowFrame)
    {
        tc.setVPos(tc.getPos());
        tc.setVScale(tc.getScale());
    }
    else if (parentTreeStruct && parentTreeStruct->getType() == HkNodeType::RootWindowFrame)
    {
        /* Minimize only container of windowframe */
        if (sceneDataRef_.isSceneMinimized && treeStruct_.getType() == HkNodeType::Container)
        {
            tc.setVPos({ 0,0 });
            tc.setVScale({ 0,0 });
        }
        else
        {
            tc.setVPos(tc.getPos());
            tc.setVScale(tc.getScale());
        }
    }
    /* Basically use parent's visible area to bound the rendering of it's children */
    else if (parentTreeStruct && parentTreeStruct->getType() != HkNodeType::RootWindowFrame)
    {
        const auto& pTc = parentTreeStruct->getPayload()->node_.transformContext;

        HkTransformContext cx;
        cx.setPos(pTc.getVPos());
        cx.setScale(pTc.getVScale());

        /* Compute intersection TC with the parent Tc*/
        const auto bboxTc = node_.transformContext.computeBBoxWith(cx);
        node_.transformContext.setVPos(bboxTc.pos);
        node_.transformContext.setVScale(bboxTc.scale);
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

    auto& children = treeStruct_.getChildren();

    /* We don't need to update children's transform data in these events*/
    if (sceneDataRef_.currentEvent != HkEvent::FocusScan
        && sceneDataRef_.currentEvent != HkEvent::HoverScan
        && sceneDataRef_.currentEvent != HkEvent::DropPath)
    {
        /* Resolve child constraints relative to parent */
        resolveChildrenConstraints(children, {});
    }

    /* Update children */
    for (uint32_t i = 0;i < children.size(); i++)
    {
        children[i]->getPayload()->updateMySelf();
    }
}

/* Resolve constraints based on set policy on this node */
void HkNodeBase::resolveChildrenConstraints(HkTreeStruct& children, const HkScrollbarsSize& sbSizes)
{
    node_.constraintContext.resolveConstraints(children, sbSizes);
}

/* Try figure out if im the hovered one */
void HkNodeBase::resolveHover()
{
    // we need the last scrollable conte    nt weve met
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
    if (treeStruct_.getType() == HkNodeType::ScrollBar ||
        ((false || node_.styleContext.isOverflowAllowedY())
            && (false || node_.constraintContext.overflowXYSize_.y)))
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
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.lastActiveMouseButton == HkMouseButton::Left
        && node_.transformContext.isPosInsideOfNodeViewableArea(sceneDataRef_.mousePos))
    {
        sceneDataRef_.focusedId = treeStruct_.getId();
        sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.getPos() - sceneDataRef_.mousePos;
    }
}

/* Resolve specific and general mouse click evt */
void HkNodeBase::resolveMouseClickEvent()
{
    //TODO: We shall handle the mouse buttons too somehow..not only the left one

    /* Notify click on actually clicked object only*/
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId == treeStruct_.getId()) // maybe the hovered one? not the focused one?
    {
        onClick();
        node_.eventsContext.invokeMouseEvent(sceneDataRef_.mousePos.x, sceneDataRef_.mousePos.x,
            HkMouseAction::Click, sceneDataRef_.lastActiveMouseButton);
    }
    /* Notify release on actually released object only*/
    else if (!sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId == treeStruct_.getId())
    {
        onRelease();
        node_.eventsContext.invokeMouseEvent(sceneDataRef_.mousePos.x, sceneDataRef_.mousePos.x,
            HkMouseAction::Release, sceneDataRef_.lastActiveMouseButton);
    }

    /* Then notify the rest */
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.hoveredId != treeStruct_.getId())
        onGeneralMouseClick();
}

/* Resolve specific and general mouse scroll evt */
void HkNodeBase::resolveMouseScrollEvent()
{
    /* Notify scroll on actually scrolled object only*/
    if (sceneDataRef_.hoveredId == treeStruct_.getId())
    {
        onScroll();
        node_.eventsContext.invokeMouseEvent(0, sceneDataRef_.scrollPosY,
            HkMouseAction::Scroll, HkMouseButton::None);
    }
    /* Then notify the rest */
    else
    {
        //TODO: Don't really know how useful it really is
        onGeneralMouseScroll();
    }
}

/* Resolve specific and general mouse mvmt evt */
void HkNodeBase::resolveMouseMovementEvent()
{
    //TODO: It works here but all shaders need to have hovered uniform
    // if (treeStruct_.getType() == HkNodeType::Container)
    // {
    //     if (sceneDataRef_.hoveredId == treeStruct_.getId())
    //     {
    //         // node_.renderContext.getShader().setInt("hovered", 1);
    //     }
    //     else
    //     {
    //         // node_.renderContext.getShader().setInt("hovered", 0);
    //     }
    // }
    /* If scene detected a dragging action, separatelly specify that dragged element*/
    if (sceneDataRef_.isDragging && sceneDataRef_.focusedId == treeStruct_.getId())
        onDrag();

    /* And notify in general the rest of the tree objects. Dragged+Selected node will get a call from
       this function too.*/
    onGeneralMouseMove();

    /* Here we should already know for sure who's the hovered element */
    if (sceneDataRef_.hoveredId == treeStruct_.getId())
    {
        node_.eventsContext.invokeMouseEvent(sceneDataRef_.mousePos.x, sceneDataRef_.mousePos.x,
            HkMouseAction::Move, HkMouseButton::None);
    }
}

/* Events to be consumed by derived if needed */
void HkNodeBase::postRenderAdditionalDetails() {}
void HkNodeBase::onDrag() {}
void HkNodeBase::onClick() {}
void HkNodeBase::onRelease() {}
void HkNodeBase::onScroll() {}
void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
void HkNodeBase::onGeneralMouseScroll() {}

/* Public Getters */
HkStyleContext& HkNodeBase::getStyle() { return node_.styleContext; }

HkEventsContext& HkNodeBase::getEvents() { return node_.eventsContext; }

HkNodeInfo HkNodeBase::getNodeInfo()
{
    return { treeStruct_.getName(), treeStruct_.getType(), treeStruct_.getId() };
};
} // hkui