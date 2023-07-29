#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNodeBase(windowName, "RootWindowFrame")
    // , wfContainer(std::make_shared<HkWFContainer>("WindowFrame_WfContainer"))
    // , exitBtnNode(std::make_shared<HkButton>("ControlCont_ExitBtn"))
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.9f)); // BLUEish
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
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
        /*
        If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
        candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
        will actually be one of the leaf of the UI tree who hits and validates this check the latest. */
        // if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
        // {
        //     sceneDataRef.maybeSelectedNodeId = getId();
        // }
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    // /*Don't forget to show node + additional details & update children*/
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    // renderAdditionalSelfElements();
    updateChildren();

    // std::cout << "Diff: " << transformContext.getDiff().x << " " << transformContext.getDiff().y << '\n';

    /*Clear my movement direction after frame ended for this node*/
    // transformContext.clearDiff();
}

void HkWindowFrame::updateChildren()
{
    for (const auto& child : treeStruct_.getChildren())
        child->getPayload()->updateMySelf();
}

void HkWindowFrame::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        treeStruct_.pushChild(&child->treeStruct_);
        // treeStruct.removeChildren({ child->treeStruct.getId() });
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
    node_.transformContext.setScale(size);
}

} // hkui
