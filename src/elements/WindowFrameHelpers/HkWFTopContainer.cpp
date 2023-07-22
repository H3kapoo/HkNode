#include "HkWFTopContainer.hpp"

namespace hkui
{

HkWFTopContainer::HkWFTopContainer(const std::string& topContainerName)
    : HkNode(topContainerName, "WindowFrameTopContainer")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
{
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
}

void HkWFTopContainer::updateMySelf()
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
        rubber band to center of UI node and instead it keeps a natural offset to it. This is used for grabbing.*/
        if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
        {
            sceneDataRef.maybeSelectedNodeId = getId();
            sceneDataRef.mouseOffsetFromCenter = transformContext.getPos() - sceneDataRef.mousePos;
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
    /*Don't forget to show node*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    // updateChildren();
}


} // hkui