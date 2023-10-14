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

    exitBtn_.setOnClickListener([this]()
        {
            stillAlive_ = false;
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
    startPos = node_.transformContext.getPos();
    endPos = windowDataPtr_->mouseOffsetFromFocusedCenter + windowDataPtr_->mousePos;
    isAnimOngoing = true;
    restarted = true;

    // node_.transformContext.setPos(endPos);

}

void HkWindowFrame::onGeneralMouseClickOrRelease()
{}

void HkWindowFrame::onGeneralMouseMove()
{
    if (!windowDataPtr_->isMouseClicked)
    {
        const auto mousePos = windowDataPtr_->mousePos;
        const auto nodeEndPos = node_.transformContext.getPos().x + node_.transformContext.getScale().x;
        const auto nodeEndPosY = wfCont_.node_.transformContext.getPos().y + wfCont_.node_.transformContext.getScale().y;

        const bool TZone = (mousePos.y > node_.transformContext.getPos().y - grabOffset)
            && (mousePos.y < node_.transformContext.getPos().y);
        const bool BZone = (mousePos.y < nodeEndPosY + grabOffset)
            && (mousePos.y > nodeEndPosY);
        const bool LZone = (mousePos.x > node_.transformContext.getPos().x - grabOffset)
            && (mousePos.x < node_.transformContext.getPos().x);
        const bool RZone = (mousePos.x > nodeEndPos) && (mousePos.x < nodeEndPos + grabOffset);

        const bool VBound = mousePos.y > node_.transformContext.getPos().y &&
            mousePos.y < nodeEndPosY;
        const bool HBound = mousePos.x > node_.transformContext.getPos().x &&
            mousePos.x < nodeEndPos;

        // pinch right
        if (RZone && VBound)
        {
            /* This shall be moved out of here to first heart beat maybe*/
            if (!cursorH)
            {
                cursorH = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
            }
            std::cout << "right edge click\n";
            lockedInXR = true;
        }
        else
        {
            lockedInXR = false;
        }

        // pinch left
        if (LZone && VBound)
        {
            /* This shall be moved out of here to first heart beat maybe*/
            if (!cursorH)
            {
                cursorH = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
            }
            std::cout << "left edge click\n";
            lockedInXL = true;
        }
        else
        {
            lockedInXL = false;
        }

        // pinch top
        if (TZone && HBound)
        {
            if (!cursorV)
            {
                cursorV = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
            }
            std::cout << "top edge click\n";
            lockedInYT = true;
        }
        else
        {
            lockedInYT = false;
        }

        // pinch bottom
        if (BZone && HBound)
        {
            if (!cursorV)
            {
                cursorV = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
            }
            std::cout << "bottom edge click\n";
            lockedInYB = true;
        }
        else
        {
            lockedInYB = false;
        }

        // diagonal bottom-right pinch
        if (RZone && BZone)
        {
            if (!cursorHV)
            {
                cursorHV = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
            }
            lockedInXR = true;
            lockedInYB = true;
        }

        // diagonal top-right pinch
        if (RZone && TZone)
        {
            if (!cursorHV)
            {
                cursorHV = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
            }
            lockedInXR = true;
            lockedInYT = true;
        }

        // diagonal bottom-left pinch
        if (LZone && BZone)
        {
            if (!cursorHV)
            {
                cursorHV = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
            }
            lockedInXL = true;
            lockedInYB = true;
        }

        // diagonal top-left pinch
        if (LZone && TZone)
        {
            std::cout << "possible top left pinch\n";
            if (!cursorHV)
            {
                cursorHV = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
            }
            lockedInXL = true;
            lockedInYT = true;
        }
    }

    if (lockedInXR && lockedInYB)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorHV);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addScale(
                { windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x, 0 });
            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    (windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
        return;
    }

    if (lockedInXR && lockedInYT)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorHV);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addScale(
                { windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x, 0 });
            node_.transformContext.addPos(
                {
                    0,
                    windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y
                });

            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    -(windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
        return;
    }

    if (lockedInXL && lockedInYB)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorHV);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addPos(
                { (windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });

            node_.transformContext.addScale(
                { -(windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });
            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    (windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
        return;
    }

    if (lockedInXL && lockedInYT)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorHV);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addPos(
                { (windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });

            node_.transformContext.addScale(
                { -(windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });
            node_.transformContext.addPos(
                {
                    0,
                    windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y
                });

            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    -(windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
        return;
    }

    if (lockedInXR)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorH);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addScale(
                { windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x, 0 });
        }
    }

    if (lockedInXL)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorH);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addPos(
                { (windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });

            node_.transformContext.addScale(
                { -(windowDataPtr_->mousePos.x - windowDataPtr_->lastMousePos.x), 0 });
        }
    }

    if (lockedInYT)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorV);
        if (windowDataPtr_->isMouseClicked)
        {
            node_.transformContext.addPos(
                {
                    0,
                    windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y
                });

            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    -(windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
    }

    if (lockedInYB)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, cursorV);
        if (windowDataPtr_->isMouseClicked)
        {
            wfCont_.node_.transformContext.addScale(
                {
                    0,
                    (windowDataPtr_->mousePos.y - windowDataPtr_->lastMousePos.y)
                });
        }
    }

    /* If we didn't manage to grab anything, reset cursor */
    if (lockedInXR == false && lockedInXL == false && lockedInYT == false && lockedInYB == false)
    {
        glfwSetCursor(windowDataPtr_->windowHandle, NULL);
    }
}

void HkWindowFrame::onClick()
{
    clickedPos = windowDataPtr_->mousePos;
    std::cout << "clicked\n";

    // // pinch right
    // const auto nodeEndPos = node_.transformContext.getPos().x + node_.transformContext.getScale().x;
    // if (clickedPos.x > nodeEndPos - 15 && !lockedInXR)
    // {
    //     std::cout << "right edge click\n";
    //     lockedInXR = true;
    //     return;
    // }

    // // pinch left
    // if (clickedPos.x < node_.transformContext.getPos().x + 15 && !lockedInXL)
    // {
    //     std::cout << "left edge click\n";
    //     lockedInXL = true;
    //     return;
    // }

    // // pinch top
    // if (clickedPos.y < node_.transformContext.getPos().y + 15 && !lockedInYT)
    // {
    //     std::cout << "top edge click\n";
    //     lockedInYT = true;
    //     return;
    // }
}

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
    minimizeBtn_.windowDataPtr_ = windowDataPtr;
    exitBtn_.windowDataPtr_ = windowDataPtr;
    wfCont_.windowDataPtr_ = windowDataPtr;
}

bool HkWindowFrame::isAlive() const
{
    return stillAlive_;
}
} // hkui
