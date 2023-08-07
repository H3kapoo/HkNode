#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNodeBase(windowName, "RootWindowFrame")
    , wfCont_("{Internal}-ContainerFor " + windowName)
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.9f)); // BLUEish
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    treeStruct_.pushChild(&wfCont_.treeStruct_);

    wfCont_.scrollbars(true, true);
}

void HkWindowFrame::rootUpdateMySelf() { updateMySelf(); }

void HkWindowFrame::onGeneralMouseMove()
{
    // /* Safe to assume that this is what dragging the current element logic looks like */
    // //TODO: A dragging state needs to be added in SM => added, just adapt THIS code
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.focusedId == treeStruct_.getId())
    {
        node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromFocusedCenter + sceneDataRef_.mousePos);
    }
}

void HkWindowFrame::onWindowResize()
{
    /* Techically root windows shall not resize with WINDOW itself, only children should resize with their parents */
    //TODO: Future: Refactor transforms so that they have pivot at top left corner instead of center
    // auto factor = 0.5f;
    // auto factor2 = 0.25f;
    // node_.transformContext.setScale({ sceneDataRef_.windowWidth * factor, 30 });
    // node_.transformContext.setPos({ sceneDataRef_.windowWidth * factor, sceneDataRef_.windowHeight * factor2 });
    // wfCont_.node_.transformContext.setScale({ sceneDataRef_.windowWidth * factor, sceneDataRef_.windowHeight * factor });
}

void HkWindowFrame::onGeneralMouseClick()
{}

void HkWindowFrame::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    node_.constraintContext.windowFrameContainerConstraint(wfCont_.node_.transformContext);
}

void HkWindowFrame::onGeneralUpdate()
{}

void HkWindowFrame::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    wfCont_.pushChildren(newChildren);
}

void HkWindowFrame::printTree() {
    treeStruct_.printTree();
}

void HkWindowFrame::setColor(const glm::vec3& color)
{
    node_.renderContext.shader.setVec3f("color", color);
}

void HkWindowFrame::setPos(const glm::vec2& pos) {
    node_.transformContext.setPos(pos);
}

void HkWindowFrame::setSize(const glm::vec2& size) {
    node_.transformContext.setScale({ size.x, 30 });
    wfCont_.node_.transformContext.setScale(size);
}

void HkWindowFrame::setConstraintPolicy(const HkConstraintPolicy policy)
{
    wfCont_.node_.constraintContext.setPolicy(policy);
}
} // hkui
