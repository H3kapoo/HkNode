#include "HkWindowFrame.hpp"

namespace hkui
{

HkWindowFrame::HkWindowFrame(const std::string& windowName)
    : HkNode(windowName, "RootFrame")
    , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
    , elementsInitialized(false)
    , wfContainer(std::make_shared<HkWFContainer>("WindowFrame_WfContainer"))
    , exitBtnNode(std::make_shared<HkButton>("ControlCont_ExitBtn"))
{
    renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    renderContext.shader.setVec3f("color", glm::vec3(0.0f, 0.5f, 0.5f)); // BLUEish
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
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

    /*Don't forget to show node + additional details & update children*/
    renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    renderAdditionalSelfElements();
    updateChildren();

    // std::cout << "Diff: " << transformContext.getDiff().x << " " << transformContext.getDiff().y << '\n';

    /*Clear my movement direction after frame ended for this node*/
    transformContext.clearDiff();
}

void HkWindowFrame::renderAdditionalSelfElements()
{
    /* This is a side hack due to HkNode inheriting shared_from_this..we cannot do the shared_ptr creation inside
       the constructor and also the push.. */
    if (!elementsInitialized)
    {
        /* Set offset from center of window frame + scale control cont after window*/
        wfContainer->transformContext.setScale({ transformContext.getScale().x, 30 });
        wfContainer->transformContext.init(
            {
                transformContext.getPos().x,
                transformContext.getPos().y - transformContext.getScale().y / 2 - 15
            });

        elementsInitialized = true;
    }

    wfContainer->updateMySelf();

    /*If parent moves, it will generate a XY vector representing direction of movement. Logic here is to move
      children in the same direction the same amount. To not move continously this amount, we need to clear the
      previously computed diff.*/
    transformContext.addPos(wfContainer->transformContext.getDiff());
    wfContainer->transformContext.clearDiff();
}

void HkWindowFrame::updateChildren()
{
    for (const auto& child : wfContainer->getChildren())
        child->updateMySelf();
}

void HkWindowFrame::pushChildren(const std::vector<HkNodePtr>& newChildren)
{
    wfContainer->pushChildren(newChildren);
}

void HkWindowFrame::printTree() { HkNode::printTree(); }

void HkWindowFrame::setColor(const glm::vec3& color)
{
    renderContext.shader.setVec3f("color", color);
}

/* Here it is not necessary to use TC.init() for first initialization as diff is not used by anyone, so it can be left
to be as big as it want in the first frame */
void HkWindowFrame::setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

void HkWindowFrame::setSize(const glm::vec2& size) { transformContext.setScale(size); }

} // hkui
