#include "HkWindowFrame.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNodeBase(windowName, HkNodeType::RootWindowFrame)
    , minimizeBtn_("{Internal}-MinimizeButtonFor " + windowName)
    , exitBtn_("{Internal}-ExitButtonFor " + windowName)
    , wfCont_("{Internal}-ContainerFor " + windowName)
    , mode_{ HkWindowFrameMode::Grabbable }
    , cachedScale_{ 680, 480 } //TODO: hack just for now
    , cachedPos_{ 0,0 }
{
    /* Setup defaults that don't have to do with VAOs/Textures/Shaders themselves*/
    node_.styleContext.setColor(glm::vec3(0.0f, 0.5f, 0.9f));
    wfCont_.node_.styleContext.setRowWrapping(true);
    wfCont_.node_.styleContext.setColor(glm::vec3(0.0f, 0.5f, 0.5f));


    treeStruct_.pushChild(&minimizeBtn_.treeStruct_);
    treeStruct_.pushChild(&exitBtn_.treeStruct_);
    treeStruct_.pushChild(&wfCont_.treeStruct_);

    minimizeBtn_.setOnClickListener([this]()
        {
            isMinimized_ = !isMinimized_;
        });

    // exitBtn_.setOnClickListener([this]()
    //     {
    //         stillAlive_ = false;
    //     });

    exitBtn_.setOnClickListener([this]()
        {
            dummyVal = !dummyVal;
            if (dummyVal) // fooling around, rm later
            {
                node_.styleContext.setColor({ 1.0f,0.0f,0.0f });
            }
            else
            {
                node_.styleContext.setColor({ 0.0f,0.0f,1.0f });
            }
            std::cout << "changed mode\n";
        });
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
    // std::cout << glfwGetTime() << " scroll value: " << windowDataPtr_->scrollPosY << '\n';
    node_.transformContext.addScale({ windowDataPtr_->scrollPosY * 4, 0 });
}

void HkWindowFrame::onDrag()
{
    if (mode_ != HkWindowFrameMode::Grabbable) return;
    if (dummyVal)
    {
        isAnimOngoing = false;
        node_.transformContext.setPos(windowDataPtr_->mouseOffsetFromFocusedCenter + windowDataPtr_->mousePos);
    }
    else
    {
        startPos = node_.transformContext.getPos();
        endPos = windowDataPtr_->mouseOffsetFromFocusedCenter + windowDataPtr_->mousePos;
        isAnimOngoing = true;
        restarted = true;
    }
}

void HkWindowFrame::onWindowResize()
{
    // setPos({ 0,0 });
    // setSize({ windowDataPtr_->windowWidth, windowDataPtr_->windowHeight - 30 });
    //TODO: Refactor if needed after topLeft coordinate change
    /* Techically root windows shall not resize with WINDOW itself, only children should resize with their parents */
    //TODO: Future: Refactor transforms so that they have pivot at top left corner instead of center
    // auto factor = 0.5f;
    // auto factor2 = 0.25f;
    // //Just for now
    // auto width_factor = std::round(windowDataPtr_->windowWidth * factor / 2) * 2;
    // auto height_factor2 = std::round(windowDataPtr_->windowHeight * factor2 / 2) * 2;
    // auto height_factor = std::round(windowDataPtr_->windowHeight * factor / 2) * 2;

    // node_.transformContext.setScale({ windowDataPtr_->windowWidth * factor, 30 });
    // node_.transformContext.setPos({ windowDataPtr_->windowWidth * factor, windowDataPtr_->windowHeight * factor });
    // wfCont_.node_.transformContext.setScale({ windowDataPtr_->windowWidth * factor, wfCont_.node_.transformContext.scale.y });

}

void HkWindowFrame::resolveChildrenConstraints(HkTreeStruct&,
    const HkScrollbarsSize&)
{
    /* If we go into fullscreen mode, remember our grabbable size and scale to help restore later */
    if (cachedScale_.x == 0 && mode_ != HkWindowFrameMode::Grabbable)
    {
        cachedPos_ = wfCont_.node_.transformContext.getPos();
        cachedScale_ = wfCont_.node_.transformContext.getScale();
    }


    node_.constraintContext.windowFrameContainerConstraint(wfCont_.node_.transformContext,
        exitBtn_.node_.transformContext, minimizeBtn_.node_.transformContext, windowDataPtr_->windowSize,
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

HkStyleContext& HkWindowFrame::getStyle()
{
    return wfCont_.getStyle();
}

void HkWindowFrame::rootUpdate() { updateMySelf(isMinimized_); renderMySelf(); }

HkNodeBase* HkWindowFrame::getUnderlayingNode() { return this; }

void HkWindowFrame::injectWindowDataPtr(HkWindowData* windowDataPtr)
{
    /* Window frame is special and has children UI elements,
       each of them need to know about the same window data*/
    windowDataPtr_ = windowDataPtr;
    minimizeBtn_.windowDataPtr_ = windowDataPtr;
    exitBtn_.windowDataPtr_ = windowDataPtr;
    wfCont_.windowDataPtr_ = windowDataPtr;
}

bool HkWindowFrame::isAlive() const
{
    return stillAlive_;
}
} // hkui
