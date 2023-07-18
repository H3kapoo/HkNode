#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string windowName)
    : HkNode(windowName, "RootFrame")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
    , controlFrame(std::make_shared<HkContainer>("WindowFrame_ControlFrame"))
    , exitBtnNode(std::make_shared<HkButton>("ControlFrame_ExitBtn"))
{
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.5f)); // BLUEish
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());

    // controlFrame->pushChildren({ exitBtnNode });
    if (controlFrame)
    {
        std::cout << "exists\n";
    }
    // pushChildren({ controlFrame });

}

// IHkRootNode
void HkWindowFrame::rootUpdateMySelf() { updateMySelf(); }

// HkNode
void HkWindowFrame::updateMySelf()
{
    /*main HkEvents handler*/
    switch (sceneDataRef.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize: break;
    case HkEvent::MouseMove:
        /* Safe to assume that this is what dragging the current element logic looks like */
        if (sceneDataRef.isMouseClicked && sceneDataRef.maybeFocusedNodeId == getId())
        {
            transformContext.setPos(sceneDataRef.mouseOffsetFromCenter + sceneDataRef.mousePos);
        }
        break;
    case HkEvent::MouseClick:

        /*
        If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
        candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
        will actually be one of the leafs of the UI tree who's hit and validates this check the latest.
        Offset from mouse position to UI node will also be calculated so on mouse move, object doesn't just
        rubber band to center of UI node and instead it keeps a natural offset to it. */
        if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
        {
            sceneDataRef.maybeSelectedNodeId = getId();
            sceneDataRef.mouseOffsetFromCenter = transformContext.getPos() - sceneDataRef.mousePos;
        }
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    /*Don't forget to show node & update children*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    updateChildren();
    /*Render additional things AFTER children so that additional things appear on top of children*/
    renderAdditionalSelfElements();
}

void HkWindowFrame::renderAdditionalSelfElements()
{
    /*TODO: Possible optimization would be to only to this Set on demand*/
    controlFrame->transformContext.setScale({ transformContext.getScale().x, 30 });
    controlFrame->transformContext.setPos(
        {
            transformContext.getPos().x,
            transformContext.getPos().y - transformContext.getScale().y / 2 + 15
        });

    controlFrame->updateMySelf();
    // controlFrame->printTree();
}

void HkWindowFrame::updateChildren()
{
    for (const auto& child : getChildren())
        child->updateMySelf();
}

void HkWindowFrame::pushChildren(const std::vector<HkNodePtr>& newChildren)
{
    HkNode::pushChildren(newChildren);
}

void HkWindowFrame::printTree() { HkNode::printTree(); }

void HkWindowFrame::setColor(const glm::vec3& color)
{
    renderContext.shader.setVec3f("color", color);
}

void HkWindowFrame::setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

void HkWindowFrame::setSize(const glm::vec2& size) { transformContext.setScale(size); }

} // hkui
