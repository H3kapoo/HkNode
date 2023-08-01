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
}

// IHkRootNode
void HkWindowFrame::rootUpdateMySelf() { updateMySelf(); }

void HkWindowFrame::updateMySelf()
{
    // /*main HkEvents handler*/
    switch (sceneDataRef_.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::FocusHoverScan:
        if (sceneDataRef_.isMouseClicked && sceneDataRef_.clickedMouseButton == HkMouseButton::Left
            && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
        {
            sceneDataRef_.focusedIdAux = treeStruct_.getId();
            sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.getPos() - sceneDataRef_.mousePos;
        }

        if (node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
        {
            sceneDataRef_.hoveredId = treeStruct_.getId();
        }
        break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize: break;
    case HkEvent::MouseMove:
        /* Safe to assume that this is what dragging the current element logic looks like */
        if (sceneDataRef_.isMouseClicked && sceneDataRef_.focusedId == treeStruct_.getId())
        {
            node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromFocusedCenter + sceneDataRef_.mousePos);
        }
        break;
    case HkEvent::MouseClick:
        // if (sceneDataRef_.maybeFocusedNodeId == treeStruct_.getId() && sceneDataRef_.isMouseClicked
        //     && sceneDataRef_.clickedMouseButton == HkMouseButton::Right)
        // {
        //     std::cout << "Right mouse clicked on " << treeStruct_.getName() << '\n';
        // }

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
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    updateChildren();
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
