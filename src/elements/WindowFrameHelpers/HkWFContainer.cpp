#include "HkWFContainer.hpp"

namespace hkui
{

HkWFContainer::HkWFContainer(const std::string& topContainerName)
    : HkNode(topContainerName, "WindowFrameTopContainer")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
{
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
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

    /*Theoretically this shouldnt have any parent*/
    if (const auto& p = getParent().lock())
    {
        transformContext.setPos(p->transformContext.getPos());
    }

    /*main HkEvents handler*/
    switch (sceneDataRef.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::GeneralUpdate: break;
    case HkEvent::WindowResize:
        /*
        For now this just keeps the element inside the window if the window is rescaled. Does
        not modify the size of UI elements */
        if (transformContext.getPos().x > sceneDataRef.windowWidth)
        {
            transformContext.setPos({ sceneDataRef.windowWidth, transformContext.getPos().y });
        }
        break;
    case HkEvent::MouseMove:
        break;
    case HkEvent::MouseClick:
        break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }
    /*Don't forget to show node*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    // updateChildren();
}

void HkWFContainer::updateChildren()
{
    // BIG ERROR: CANNOT ACCESS CHILDREN OF CHILDREN NODES FROM HERE, NEEDS REDESIGN
    for (const auto& child : getChildren())
        child->updateMySelf();
}

void HkWFContainer::pushChildren(const std::vector<HkNodePtr>& newChildren)
{
    HkNode::pushChildren(newChildren);
}


} // hkui