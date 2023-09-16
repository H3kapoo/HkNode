#include "HkWindowFrame.hpp"

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
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    node_.styleContext.setColor(glm::vec3(0.0f, 0.5f, 0.9f));

    wfCont_.node_.styleContext.setRowWrapping(true);
    treeStruct_.pushChild(&minimizeBtn_.treeStruct_);
    treeStruct_.pushChild(&exitBtn_.treeStruct_);
    treeStruct_.pushChild(&wfCont_.treeStruct_);

    minimizeBtn_.setOnClickListener([this]()
        {
            sceneDataRef_.isSceneMinimized = !sceneDataRef_.isSceneMinimized;
        });

    exitBtn_.setOnClickListener([this]()
        {
            sceneDataRef_.isSceneStillAlive = false;
        });
}

void HkWindowFrame::rootUpdate() { updateMySelf(); renderMySelf(); }

HkNodeBase* HkWindowFrame::getUnderlayingNode() { return this; }

void HkWindowFrame::onScroll()
{
    // std::cout << glfwGetTime() << " scroll value: " << sceneDataRef_.scrollPosY << '\n';
    node_.transformContext.addScale({ sceneDataRef_.scrollPosY * 4, 0 });
}

void HkWindowFrame::onDrag()
{
    if (mode_ != HkWindowFrameMode::Grabbable) return;
    node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromFocusedCenter + sceneDataRef_.mousePos);
}

void HkWindowFrame::onWindowResize()
{
    // setPos({ 0,0 });
    // setSize({ sceneDataRef_.windowWidth, sceneDataRef_.windowHeight - 30 });
    //TODO: Refactor if needed after topLeft coordinate change
    /* Techically root windows shall not resize with WINDOW itself, only children should resize with their parents */
    //TODO: Future: Refactor transforms so that they have pivot at top left corner instead of center
    // auto factor = 0.5f;
    // auto factor2 = 0.25f;
    // //Just for now
    // auto width_factor = std::round(sceneDataRef_.windowWidth * factor / 2) * 2;
    // auto height_factor2 = std::round(sceneDataRef_.windowHeight * factor2 / 2) * 2;
    // auto height_factor = std::round(sceneDataRef_.windowHeight * factor / 2) * 2;

    // node_.transformContext.setScale({ sceneDataRef_.windowWidth * factor, 30 });
    // node_.transformContext.setPos({ sceneDataRef_.windowWidth * factor, sceneDataRef_.windowHeight * factor });
    // wfCont_.node_.transformContext.setScale({ sceneDataRef_.windowWidth * factor, wfCont_.node_.transformContext.scale.y });

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
        exitBtn_.node_.transformContext, minimizeBtn_.node_.transformContext, sceneDataRef_.windowSize,
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
} // hkui
