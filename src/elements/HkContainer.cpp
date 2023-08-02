#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string&)
    :
    // HkNode(name, "Container")
    sceneDataRef(HkSceneManagement::get().getSceneDataRef())
{
    // renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    // renderContext.shader.setVec3f("color", glm::vec3(0.0f, 1.0f, 0.0f)); // GREEN
    // renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
}

//HkNode
void HkContainer::updateMySelf()
{
    // /*
    // I need to reposition myself based on my parent.
    // This system implicitly doesn't let you move UI elements who happen to have a parent
    // so only top level root objects can be moved.
    // The only way to move the children is by offseting them in relation to their parent.
    // Note: If this happend to be the selectedNodeId AND it has a parent, because if has a parent
    // node will be unable to move on mouse click+drag for exaple because it is constrained to the parent */
    // if (const auto& p = getParent().lock())
    // {
    //     transformContext.setPos(p->transformContext.getPos());
    //     transformContext.addPos({ 0, 130 });
    // }

    // /*main HkEvents handler*/
    // switch (sceneDataRef.currentEvent)
    // {
    // case HkEvent::None: break;
    // case HkEvent::GeneralUpdate: break;
    // case HkEvent::WindowResize:
    //     /*
    //     For now this just keeps the element inside the window if the window is rescaled. Does
    //     not modify the size of UI elements */
    //     if (transformContext.getPos().x > sceneDataRef.windowWidth)
    //     {
    //         transformContext.setPos({ sceneDataRef.windowWidth, transformContext.getPos().y });
    //     }
    //     break;
    // case HkEvent::MouseMove:
    //     break;
    // case HkEvent::MouseClick:
    //     /*
    //     If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
    //     candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
    //     will actually be one of the leaf of the UI tree who hits and validates this check the latest. */
    //     if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
    //     {
    //         sceneDataRef.maybeSelectedNodeId = getId();
    //     }
    //     break;
    // case HkEvent::MouseEnterExit: break;
    // case HkEvent::MouseScroll: break;
    // case HkEvent::DropPath: break;
    // }

    // /*Don't forget to show node*/
    // renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    // updateChildren();

    /*Unless window frame, idk why you would use it*/
    /*Clear my movement direction after frame ended for this node*/
    // transformContext.clearDiff();
}

void HkContainer::updateChildren()
{
    // for (const auto& child : getChildren())
    //     child->updateMySelf();
}

void HkContainer::pushChildren(const std::vector<std::shared_ptr<HkNode>>&)
{
    // HkNode::pushChildren(newChildren);
}

/* Funcs to be removed when styling context comes */
// void HkContainer::printTree() { HkNode::printTree(); }

void HkContainer::setColor(const glm::vec3&)
{
    // renderContext.shader.setVec3f("color", color);
}

} // hkui
