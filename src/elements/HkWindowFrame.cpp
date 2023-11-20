#include "HkWindowFrame.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNodeBase(windowName, HkNodeType::RootWindowFrame)
    , titleLabel_("{Internal}-TitleLabelFor " + windowName)
    , minimizeBtn_("{Internal}-MinimizeButtonFor " + windowName)
    , exitBtn_("{Internal}-ExitButtonFor " + windowName)
    , wfCont_("{Internal}-ContainerFor " + windowName)
    , mode_{ HkWindowFrameMode::Grabbable }
    , cachedScale_{ 680, 480 } //TODO: hack just for now
    , cachedPos_{ 0,0 }
{
    /* Setup defaults that don't have to do with VAOs/Textures/Shaders themselves*/
    node_.styleContext.setColor(glm::vec3(0.0f, 0.5f, 0.9f));

    // wfCont_.node_.styleContext.setRowWrapping(true); //TODO: Why was it enabled by default?
    wfCont_.node_.styleContext.setColor(glm::vec3(0.0f, 0.5f, 0.5f));

    pinchHelper_.setGrabSize(15);

    treeStruct_.pushChild(&titleLabel_.treeStruct_);
    treeStruct_.pushChild(&minimizeBtn_.treeStruct_);
    treeStruct_.pushChild(&exitBtn_.treeStruct_);
    treeStruct_.pushChild(&wfCont_.treeStruct_);

    minimizeBtn_.setOnClickListener([this]()
        {
            isMinimized_ = !isMinimized_;
        });

    exitBtn_.setOnClickListener([this]()
        {
            stillAlive_ = false;
        });

    titleLabel_.getStyle().setColor(glm::vec3(0.0f, 0.5f, 0.8f));
    titleLabel_.getTextStyle()
        .setFontPath("assets/fonts/LiberationSerif-Regular.ttf")
        .setRenderMethod(HkTextUserConfig::HkTextRenderMethod::BITMAP)
        .setFontSize(24)
        .setWrapAtWord(true)
        .setFontColor(glm::vec3(1.0f))
        .setTextVHAlign(HkTextUserConfig::HkTextVAlign::Center, HkTextUserConfig::HkTextHAlign::Center);

    //TODO: Fix clicking and dragging on title not moving the window frame. We need to send events to parent that we are
    //      currently under drag
    // titleLabel_.getEvents().setOnClickListener([this]()
    //     {
    //         printf("Dragging\n");
    //         onDrag();
    //     });

}

void HkWindowFrame::onFirstHeartbeat()
{
    /* Here we shall put things that cannot be initialized without first having a valid opengl/glfw context */
    /*Init pinching helper*/
    pinchHelper_.init(*windowDataPtr_);
    pinchHelper_.setGrabConfig({ .allowLeft = true, .allowRight = true, .allowTop = true, .allowBottom = true });

    HkNodeBase::onFirstHeartbeat();
}

void HkWindowFrame::onResolveFocusPass()
{
    /* All information needed by pinch helper for various reasons */
    if (wfCont_.getStyle().getPinchConfig().enable)
    {
        boundPos_ = { node_.transformContext.getPos() };
        boundScale_ = {
            node_.transformContext.getScale().x,
            wfCont_.node_.transformContext.getScale().y + node_.transformContext.getScale().y
        };

        pinchHelper_.scanCustom(*windowDataPtr_, boundPos_, boundScale_);
    }
}

void HkWindowFrame::postRenderAdditionalDetails()
{
    /* Normally we would just pass a reference to pos+scale of TC, but windowFrame is special
       because it has a top frame and a container object. To note that we should always render the bars last */
    if (wfCont_.getStyle().getPinchConfig().enable)
    {
        pinchHelper_.onBarRender(*windowDataPtr_, boundPos_, boundScale_);
    }
}

void HkWindowFrame::onAnimationFrameRequested()
{
    // if anim frame is requested but we call it too fast compared to set fixedTS value,
    // we skip this animation frame and request another one
    // Assumption is that after fixedTS time we are doing this, a frame will be actually
    // updated, resetting the accumulated time back to 0 and so on
    if (!isAnimOngoing) return;

    if (restarted)
    {
        t = 0.0f;
        restarted = false;
        startTime = glfwGetTime();
    }

    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - startTime;
    t = elapsedTime / animDuration;

    if (t > 1.0f)
    {
        t = 1.0f;
        isAnimOngoing = false;
    }

    const auto easeInOutCubic = [](double x) -> double {
        const double c1 = 1.70158;
        const double c3 = c1 + 1.0;
        return 1.0 + c3 * std::pow(x - 1.0, 3.0) + c1 * std::pow(x - 1.0, 2.0);};

    glm::vec2 interpPos = startPos + (endPos - startPos) * (float)easeInOutCubic(t);
    node_.transformContext.setPos(interpPos);
    glfwPostEmptyEvent(); //TODO: This should be posted ONLY once per frame
}

void HkWindowFrame::onScroll()
{
    node_.transformContext.addScale({ windowDataPtr_->scrollPosY * 4, 0 });
}

void HkWindowFrame::onDrag()
{
    if (mode_ != HkWindowFrameMode::Grabbable) return;
    startPos = node_.transformContext.getPos();
    endPos = windowDataPtr_->mouseOffsetFromFocusedCenter + windowDataPtr_->mousePos;
    isAnimOngoing = true;
    restarted = true;
}

void HkWindowFrame::onGeneralMouseMove()
{
    if (wfCont_.getStyle().getPinchConfig().enable)
    {
        boundPos_ = { node_.transformContext.getPos() };
        boundScale_ = {
            node_.transformContext.getScale().x,
            wfCont_.node_.transformContext.getScale().y + node_.transformContext.getScale().y
        };

        if (pinchHelper_.onMouseMoveCustom(*windowDataPtr_, boundPos_, boundScale_))
        {
            node_.transformContext.setScale({ boundScale_.x, 30 });
            node_.transformContext.setPos(boundPos_);
            wfCont_.node_.transformContext.setScale({ boundScale_.x, boundScale_.y - 30 });
        }
    }
}

void HkWindowFrame::onClick()
{}

void HkWindowFrame::onRelease()
{}

void HkWindowFrame::onWindowResize() {}

void HkWindowFrame::resolveChildrenConstraints(HkTreeStruct&,
    const HkScrollbarsSize&)
{
    /* If we go into fullscreen mode, remember our grabbable size and scale to help restore later */
    if (cachedScale_.x == 0 && mode_ != HkWindowFrameMode::Grabbable)
    {
        cachedPos_ = wfCont_.node_.transformContext.getPos();
        cachedScale_ = wfCont_.node_.transformContext.getScale();
    }

    //TODO: This needs to be refactored eventually
    node_.constraintContext.windowFrameContainerConstraint(
        titleLabel_.node_.transformContext,
        wfCont_.node_.transformContext,
        exitBtn_.node_.transformContext,
        minimizeBtn_.node_.transformContext,
        windowDataPtr_->windowSize,
        /* Means we are in the fullscreen fixed mode, we hide the "grab bar"*/
        (mode_ != HkWindowFrameMode::Grabbable ? true : false));
}


void HkWindowFrame::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    wfCont_.pushChildren(newChildren);
}

void HkWindowFrame::printTree()
{
    treeStruct_.printTree();
}

void HkWindowFrame::setColor(const glm::vec3& color)
{
    node_.styleContext.setColor(color);
}

void HkWindowFrame::setPos(const glm::vec2& pos)
{
    /* If we are not in grabbable mode, pos cannot be determined by user*/
    if (mode_ != HkWindowFrameMode::Grabbable) return;
    node_.transformContext.setPos(pos);
}

void HkWindowFrame::setSize(const glm::vec2& size)
{
    /* If we are not in grabbable mode, size cannot be determined by user*/
    if (mode_ != HkWindowFrameMode::Grabbable) return;
    node_.transformContext.setScale({ size.x, 30 });
    wfCont_.node_.transformContext.setScale(size);
}

void HkWindowFrame::setWindowMode(const HkWindowFrameMode mode)
{
    mode_ = mode;
    /* Not fully necessary, but a good cleanup */
    if (mode_ != HkWindowFrameMode::Grabbable)
    {
        treeStruct_.removeChildren({ exitBtn_.treeStruct_.getId(), minimizeBtn_.treeStruct_.getId() });
    }
    else
    {
        setSize(cachedScale_);
        setPos(cachedPos_);
        cachedScale_.x = 0;// reset cache hack
        treeStruct_.pushChild(&minimizeBtn_.treeStruct_);
        treeStruct_.pushChild(&exitBtn_.treeStruct_);
    }
}

void HkWindowFrame::setTitle(const std::string& title) { titleLabel_.setText(title); }

HkStyleContext& HkWindowFrame::getStyle()
{
    return wfCont_.getStyle();
}

const HkTransformContext& HkWindowFrame::getTransform() const
{
    return wfCont_.getTransform();
}

void HkWindowFrame::rootUpdate() { updateMySelf(isMinimized_); renderMySelf(); }

HkNodeBase* HkWindowFrame::getUnderlayingNode() { return this; }

void HkWindowFrame::injectWindowDataPtr(HkWindowData* windowDataPtr)
{
    /* Window frame is special and has children UI elements,
       each of them need to know about the same window data*/
    windowDataPtr_ = windowDataPtr;
    titleLabel_.windowDataPtr_ = windowDataPtr;
    minimizeBtn_.windowDataPtr_ = windowDataPtr;
    exitBtn_.windowDataPtr_ = windowDataPtr;
    wfCont_.windowDataPtr_ = windowDataPtr;
}

bool HkWindowFrame::isAlive() const
{
    return stillAlive_;
}
} // hkui
