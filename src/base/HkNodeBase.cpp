#include "HkNodeBase.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const HkNodeType& type)
    : treeStruct_(this, windowName, type)
{
    node_.constraintContext.injectTransformContext(&node_.transformContext);
    node_.constraintContext.injectStyleContext(&node_.styleContext);
    node_.renderContext.colorUniformEn = true;
}

void HkNodeBase::renderMySelf()
{
    auto& tc = node_.transformContext;

    //TODO: tc.isAnyDifference() is correct here but main() loop doesnt know not to clear screen on each pass anymore
    // A flag maybe is needed? Conditional clear?
    /* Normal rendering */
    if (tc.getVScale().x && tc.getVScale().y)
    {
        node_.renderContext.windowProjMatrix = windowDataPtr_->sceneProjMatrix;

        glEnable(GL_SCISSOR_TEST);
        if (node_.styleContext.isBorderEnabled())
        {
            auto& btc = node_.borderTransformContext;
            glScissor(
                btc.getVPos().x,
                windowDataPtr_->windowSize.y - btc.getVPos().y - btc.getVScale().y,
                btc.getVScale().x,
                btc.getVScale().y);
            windowDataPtr_->renderer.render(node_.renderContext, node_.styleContext, btc.getModelMatrix(), true);
        }

        /* We only render the visible area of the UI element as calculated in the update pass*/
        // if (treeStruct_.getType() != HkNodeType::RootWindowFrame)
        {
            glScissor(
                tc.getVPos().x,
                windowDataPtr_->windowSize.y - tc.getVPos().y - tc.getVScale().y,
                tc.getVScale().x,
                tc.getVScale().y);
        }

        windowDataPtr_->renderer.render(node_.renderContext, node_.styleContext, tc.getModelMatrix(), false);

        /* Update children. Also don't require bellow children to be rendered if parent can't be rendered itself */
        auto& children = treeStruct_.getChildren();
        for (uint32_t i = 0; i < children.size(); i++)
        {
            children[i]->getPayload()->renderMySelf();
        }
    }

    /* Use this to render additional non interactive things if needed */
    /* Note: rescissoring to original parent is needed unfortunatelly */
    glEnable(GL_SCISSOR_TEST);
    glScissor(
        tc.getVPos().x,
        windowDataPtr_->windowSize.y - tc.getVPos().y - tc.getVScale().y,
        tc.getVScale().x,
        tc.getVScale().y);

    postRenderAdditionalDetails();

    /* Disable scissors after rendering UI */
    glDisable(GL_SCISSOR_TEST);
}

/*
    Handles any event sent by the SM to the node, redering, and scissoring.
    Shall not be implemented by the child unless you really have no other choice.
*/
void HkNodeBase::updateMySelf(const bool isSubWindowMinimized)
{
    const auto& parentTreeStruct = treeStruct_.getParent();
    auto& tc = node_.transformContext;
    auto& bTc = node_.borderTransformContext;

    /* Compute renderable/inveractive area for each element */
    if (treeStruct_.getType() == HkNodeType::RootWindowFrame)
    {
        tc.setVPos(tc.getPos());
        tc.setVScale(tc.getScale());
        bTc.setVPos(bTc.getContentPos());
        bTc.setVScale(bTc.getContentScale());

        /* If we are the root window and we are minimized, minimize border vScale as well */
        isSubWindowMinimized ?
            bTc.setVScale(
                {
                    bTc.getContentScale().x,
                    node_.styleContext.getTopBorder() + node_.styleContext.getBottomBorder() + 30
                })
            : bTc.setVScale(bTc.getContentScale());
    }
    else if (parentTreeStruct && parentTreeStruct->getType() == HkNodeType::RootWindowFrame)
    {
        /* Minimize only container of windowframe */
        if (isSubWindowMinimized && treeStruct_.getType() == HkNodeType::Container)
        {
            tc.setVPos({ 0,0 });
            tc.setVScale({ 0,0 });
        }
        else
        {
            tc.setVPos({ tc.getPos() });
            tc.setVScale(tc.getScale());
        }
    }
    /* Basically use parent's visible area to bound the rendering of it's children */
    else if (parentTreeStruct && parentTreeStruct->getType() != HkNodeType::RootWindowFrame)
    {
        const auto& pTc = parentTreeStruct->getPayload()->node_.transformContext;

        HkTransformBBox pvBox;
        HkTransformBBox nBox;
        HkTransformBBox nBorderBox;

        pvBox.pos = pTc.getVPos();
        pvBox.scale = pTc.getVScale();

        nBox.pos = node_.transformContext.getContentPos();
        nBox.scale = node_.transformContext.getContentScale();

        nBorderBox.pos = node_.borderTransformContext.getContentPos();
        nBorderBox.scale = node_.borderTransformContext.getContentScale();

        /* Compute node TC  intersection with the parent Tc*/
        const auto bboxTc = HkTransformContext::computeBBoxWith(nBox, pvBox);
        node_.transformContext.setVPos(bboxTc.pos);
        node_.transformContext.setVScale(bboxTc.scale);

        /* Compute node border TC intersection with the parent Tc*/
        const auto borderBboxTc = HkTransformContext::computeBBoxWith(nBorderBox, pvBox);
        node_.borderTransformContext.setVPos(borderBboxTc.pos);
        node_.borderTransformContext.setVScale(borderBboxTc.scale);
    }

    /* Main HkEvents handler */
    switch (windowDataPtr_->currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::AnimationFrame: onAnimationFrameRequested(); break;
    case HkEvent::HoverScan: resolveHover(); break;
    case HkEvent::FocusScan: resolveFocus(); break;
    case HkEvent::GeneralUpdate: onGeneralUpdate(); break;
    case HkEvent::WindowResize: onWindowResize(); break;
    case HkEvent::MouseMove: resolveMouseMovementEvent(); break;
    case HkEvent::MouseClick: resolveMouseClickEvent(); break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: resolveMouseScrollEvent(); break;
    case HkEvent::DropPath: break;
    case HkEvent::KeyAction: onKeyAction(); break;
    case HkEvent::CharAction: onCharAction(); break;
    }

    /* Heartbeat to initialize stuff like shaders/VAO that cannot be initted on object creation because
       we might not have yet a valid window/context to do so. From this point we do have also a valid
       windowDataPtr. */
    if (!hadFirstHeartBeat_)
    {
        onFirstHeartbeat();
        hadFirstHeartBeat_ = true;
    }

    if (node_.styleContext.isDirty)
    {
        resolveDirtyAttributes();
        node_.styleContext.dirtyAttribs.clear();
        node_.styleContext.isDirty = false;
    }

    auto& children = treeStruct_.getChildren();

    /* We don't need to update children's transform data in these events*/
    if (windowDataPtr_->currentEvent != HkEvent::FocusScan
        && windowDataPtr_->currentEvent != HkEvent::HoverScan
        && windowDataPtr_->currentEvent != HkEvent::MouseMove
        && windowDataPtr_->currentEvent != HkEvent::KeyAction
        && windowDataPtr_->currentEvent != HkEvent::CharAction
        && windowDataPtr_->currentEvent != HkEvent::DropPath)
        // && windowDataPtr_->currentEvent != HkEvent::None)
    {
        /* Resolve child constraints relative to parent */
        resolveChildrenConstraints(children, {});
    }

    /* Update children */
    for (uint32_t i = 0; i < children.size(); i++)
    {
        children[i]->getPayload()->updateMySelf(isSubWindowMinimized);
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
    if (isTransparent_) { return; }

    // we need the last scrollable content weve met
    if (node_.transformContext.isPosInsideOfNodeViewableArea(windowDataPtr_->mousePos))
    {
        windowDataPtr_->hoveredId = treeStruct_.getId();
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
        windowDataPtr_->nearestScrollContainerId_ = treeStruct_.getId();
    }
}

/* Try figure out if im the focused one */
void HkNodeBase::resolveFocus()
{
    /*Element is in focus only if element is not transparent, mouse is clicked and the mouse pos is inside the element.
      Mouse offsets also get computed so is dragging occurs later on focused object, object doesn't
      just snap to clicked mouse position */
    if (isTransparent_) { return; }

    if (!isTransparent_ && windowDataPtr_->isMouseClicked && windowDataPtr_->lastActiveMouseButton == HkMouseButton::Left
        && node_.transformContext.isPosInsideOfNodeViewableArea(windowDataPtr_->mousePos))
    {
        windowDataPtr_->focusedId = treeStruct_.getId();
        windowDataPtr_->mouseOffsetFromFocusedCenter = node_.transformContext.getPos() - windowDataPtr_->mousePos;
    }

    //TODO: For pinching, maybe its best to do a similar thing that we did with nearest
    // scrollbar. Try to see if on click-focus we might be also happening to pinch something
    // IF we next 'drag'

    /* On focus we also need to resolve any pinching that might occur */
    onResolveFocusPass();
}

/* Resolve specific and general mouse click evt */
void HkNodeBase::resolveMouseClickEvent()
{
    //TODO: We shall handle the mouse buttons too somehow..not only the left one
    if (isTransparent_) { return; }

    /* Notify click on actually clicked object only*/
    if (windowDataPtr_->isMouseClicked && windowDataPtr_->hoveredId == treeStruct_.getId()) // maybe the hovered one? not the focused one?
    {
        onClick();
        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Click, windowDataPtr_->lastActiveMouseButton);
    }
    /* Notify release on actually released object only*/
    else if (!windowDataPtr_->isMouseClicked && windowDataPtr_->focusedId == treeStruct_.getId())
    {
        //TODO: Better handling of how clicks/releases are made
        onRelease();
        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Release, windowDataPtr_->lastActiveMouseButton);
    }

    /* Then notify the rest */
    onGeneralMouseClickOrRelease();
}

/* Resolve specific and general mouse scroll evt */
void HkNodeBase::resolveMouseScrollEvent()
{
    /* Notify scroll on actually scrolled object only*/
    if (windowDataPtr_->hoveredId == treeStruct_.getId())
    {
        onScroll();
        node_.eventsContext.invokeMouseEvent(0, windowDataPtr_->scrollPosY,
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
    /* Handle exit special case when prevHovered is not initialized yet*/
    if (windowDataPtr_->prevHoveredId == 0)
    {
        windowDataPtr_->prevHoveredId = windowDataPtr_->hoveredId;
        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Exited, HkMouseButton::None);
    }

    /* Handle exit event for previously hovered item */
    if (windowDataPtr_->hoveredId != windowDataPtr_->prevHoveredId && windowDataPtr_->prevHoveredId == treeStruct_.getId())
    {
        windowDataPtr_->prevHoveredId = windowDataPtr_->hoveredId;
        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Exited, HkMouseButton::None);
    }

    /* If scene detected a dragging action, separatelly specify that dragged element*/
    if (windowDataPtr_->isDragging && windowDataPtr_->focusedId == treeStruct_.getId())
        onDrag();

    /* And notify in general the rest of the tree objects. Dragged+Selected node will get a call from
       this function too.*/
    onGeneralMouseMove();

    /* Here we should already know for sure who's the hovered element */
    if (windowDataPtr_->hoveredId == treeStruct_.getId())
    {
        /* Handle enter event for hovered item. This is made internally such that event will be invoked just once,
           even tho call here happens each frame */
        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Entered, HkMouseButton::None);

        node_.eventsContext.invokeMouseEvent(windowDataPtr_->mousePos.x, windowDataPtr_->mousePos.x,
            HkMouseAction::Move, HkMouseButton::None);
    }
}

/* Resolve runtime changed attributes */
void HkNodeBase::resolveDirtyAttributes()
{
    //TODO: This could be used in constraint context as well to not render when not needed
    for (const auto& chAttrib : node_.styleContext.dirtyAttribs)
    {
        switch (chAttrib)
        {
        case HkStyleDirtyAttribs::BG:
        {
            /* Change shader to the textured one if we have background set */
            const auto& bgImagePath = node_.styleContext.getBackgroundImage();
            if (bgImagePath.size() > 0)
            {
                std::string DEFAULT_VS = "assets/shaders/vTextured.glsl"; //TODO: could be fetched from style in the future
                std::string DEFAULT_FS = "assets/shaders/fTextured.glsl";
                node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
                auto texInfo = windowDataPtr_->renderer.addTextureSourceToCache(bgImagePath);
                texInfo.texName = "texture1";
                texInfo.texUnit = GL_TEXTURE0;
                node_.renderContext.texInfos.push_back(texInfo);
                node_.renderContext.colorUniformEn = false;
            }
            else
            {
                std::string DEFAULT_VS = "assets/shaders/v1.glsl";
                std::string DEFAULT_FS = "assets/shaders/f1.glsl";
                node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
                node_.renderContext.texInfos.clear();
                node_.renderContext.colorUniformEn = true;
            }
            break;
        }
        case HkStyleDirtyAttribs::Pinch:
            // Will be resolved by each element individually. Can't really be resolved from here
            break;
        }
    }

    onDirtyAttribs(node_.styleContext.dirtyAttribs);
}

void HkNodeBase::onFirstHeartbeat()
{
    std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;
    node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);
}

void HkNodeBase::setSelectTransparent(const bool isTransparent) { isTransparent_ = isTransparent; };

/* Events to be consumed by derived if needed */
void HkNodeBase::postRenderAdditionalDetails() {}
void HkNodeBase::onAnimationFrameRequested() {}
void HkNodeBase::onDrag() {}
void HkNodeBase::onClick() {}
void HkNodeBase::onRelease() {}
void HkNodeBase::onScroll() {}
void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClickOrRelease() {}
void HkNodeBase::onGeneralMouseScroll() {}
void HkNodeBase::onResolveFocusPass() {}
void HkNodeBase::onKeyAction() {}
void HkNodeBase::onCharAction() {}
void HkNodeBase::onDirtyAttribs(const std::unordered_set<HkStyleDirtyAttribs>&) {}


/* Injects */
void HkNodeBase::injectWindowDataPtr(HkWindowData* windowDataPtr) { windowDataPtr_ = windowDataPtr; }

/* Public Getters */
HkStyleContext& HkNodeBase::getStyle() { return node_.styleContext; }

HkEventsContext& HkNodeBase::getEvents() { return node_.eventsContext; }

const HkTransformContext& HkNodeBase::getTransform() const { return node_.transformContext; }

HkNodeInfo HkNodeBase::getNodeInfo() { return { treeStruct_.getName(), treeStruct_.getType(), treeStruct_.getId() }; };
} // hkui