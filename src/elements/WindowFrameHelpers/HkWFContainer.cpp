#include "HkWFContainer.hpp"

namespace hkui
{

HkWFContainer::HkWFContainer(const std::string& containerName)
    : HkNodeBase(containerName, "[Internal] RootWindowFrame_Container")
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

void HkWFContainer::updateMySelf()
{
    /*
    I need to reposition myself based on my parent.
    This system implicitly doesn't let you move UI elements who happen to have a parent
    so only top level root objects can be moved.
    The only way to move the children is by offseting them in relation to their parent.
    Note: If this happend to be the selectedNodeId AND it has a parent, because if has a parent
    node will be unable to move on mouse click+drag for exaple because it is constrained to the parent */

    // /*Theoretically this shouldnt have any parent*/
    // if (const auto& p = getParent().lock())
    // {
    //     transformContext.setPos(p->transformContext.getPos());
    // }
    repositionBasedOnParent();

    /*main HkEvents handler*/
    switch (sceneDataRef_.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize:
        /*
        For now this just keeps the element inside the window if the window is rescaled. Does
        not modify the size of UI elements */
        // if (node_.transformContext.getPos().x > sceneDataRef_.windowWidth)
        // {
        //     node_.transformContext.setPos({ sceneDataRef_.windowWidth, node_.transformContext.getPos().y });
        // }
        break;
    case HkEvent::MouseMove:
        break;
    case HkEvent::MouseClick:
        /*
        If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
        candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
        will actually be one of the leafs of the UI tree who's hit and validates this check the latest.*/
        if (sceneDataRef_.isMouseClicked && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
        {
            sceneDataRef_.maybeSelectedNodeId = treeStruct_.getId();
        }
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }
    // /*Don't forget to show node*/
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    updateChildren();
}

void HkWFContainer::repositionBasedOnParent()
{
    /* This node type is always guaranteed to have a parent */
    HkNodeBase* parent = treeStruct_.getParent()->getPayload();
    const auto pos = parent->node_.transformContext.getPos();
    const auto scale = parent->node_.transformContext.getScale();
    const auto thisPos = node_.transformContext.getPos();
    const auto thisScale = node_.transformContext.getScale();

    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    node_.transformContext.setPos({ pos.x, pos.y + thisScale.y / 2 + scale.y / 2 });
    node_.transformContext.setScale({ scale.x, 200 });
}

void HkWFContainer::updateChildren()
{
    for (const auto& child : treeStruct_.getChildren())
        child->getPayload()->updateMySelf();
}

void HkWFContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        treeStruct_.pushChild(&child->treeStruct_);
    }
}
} // hkui