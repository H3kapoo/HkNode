#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNodeBase(windowName, "RootWindowFrame")
    , wfCont_("ContainerFor_" + windowName)
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.9f)); // BLUEish
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    treeStruct_.pushChild(&wfCont_.treeStruct_);

    rc.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    rc.shader.setVec3f("color", glm::vec3(1.0f, 0.5f, 0.9f)); // BLUEish
    rc.render(sceneDataRef_.sceneProjMatrix, tc.getModelMatrix());
}

// IHkRootNode
void HkWindowFrame::rootUpdateMySelf() { updateMySelf(); }

void HkWindowFrame::updateMySelf()
{
    // /*main HkEvents handler*/
    switch (sceneDataRef_.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize: break;
    case HkEvent::MouseMove:
        /* Safe to assume that this is what dragging the current element logic looks like */
        if (sceneDataRef_.isMouseClicked && sceneDataRef_.maybeFocusedNodeId == treeStruct_.getId())
        {
            node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromCenter + sceneDataRef_.mousePos);
        }
        break;
    case HkEvent::MouseClick:
        /*
        If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
        candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
        will actually be one of the leafs of the UI tree who's hit and validates this check the latest.
        Offset from mouse position to UI node will also be calculated so on mouse move, object doesn't just
        rubber band to center of UI node and instead it keeps a natural offset to it. This is used for grabbing.*/
        if (sceneDataRef_.isMouseClicked && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
        {
            sceneDataRef_.maybeSelectedNodeId = treeStruct_.getId();
            sceneDataRef_.mouseOffsetFromCenter = node_.transformContext.getPos() - sceneDataRef_.mousePos;
        }

        if (sceneDataRef_.maybeFocusedNodeId == treeStruct_.getId() && sceneDataRef_.isMouseClicked
            && sceneDataRef_.clickedMouseButton == HkMouseButton::Right)
        {
            std::cout << "Right mouse clicked on " << treeStruct_.getName() << '\n';
            isMinimized = !isMinimized;
        }

        /*TODO: this doesnt capture click release, fix later*/
        // if (sceneDataRef_.maybeFocusedNodeId == treeStruct_.getId() && !sceneDataRef_.isMouseClicked
        //     && sceneDataRef_.clickedMouseButton == HkMouseButton::Right)
        // {
        //     std::cout << "Right mouse released on " << treeStruct_.getName() << '\n';
        // }
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    // /*Don't forget to show node & update children*/
    if (isMinimized)
    {
        glEnable(GL_SCISSOR_TEST);
    }

    glScissor(
        node_.transformContext.getPos().x - node_.transformContext.getScale().x / 2,
        sceneDataRef_.windowHeight - node_.transformContext.getPos().y - node_.transformContext.getScale().y / 2,
        node_.transformContext.getScale().x,
        node_.transformContext.getScale().y);

    tc.setScale({ 10,10 });
    tc.setPos({
        node_.transformContext.getPos().x - node_.transformContext.getScale().x / 2,
        node_.transformContext.getPos().y + node_.transformContext.getScale().y / 2 });
    rc.render(sceneDataRef_.sceneProjMatrix, tc.getModelMatrix());

    // tc.setPos({ node_.transformContext.getPos().x + node_.transformContext.getScale().x / 2,
    //     node_.transformContext.getPos().y - node_.transformContext.getScale().y / 2 });

    // rc.render(sceneDataRef_.sceneProjMatrix, tc.getModelMatrix());

    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    updateChildren();
    glDisable(GL_SCISSOR_TEST);
}

void HkWindowFrame::updateChildren()
{
    wfCont_.updateMySelf();
}

void HkWindowFrame::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        wfCont_.treeStruct_.pushChild(&child->treeStruct_);
    }
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
} // hkui
