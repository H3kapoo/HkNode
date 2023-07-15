#pragma once

#include <string>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{

class HkContainer : public HkNode
{
public:
    HkContainer(const std::string& name)
        : HkNode(name, "Container")
        , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
    {

        /* By default position at center of parent, could be changed by setting constraints*/
        if (const auto& p = getParent().lock())
        {
            transformContext.setPos(p->transformContext.getPos());
        }
        // transformContext.setOffsetFromParent({ 0, 30 });

        transformContext.setOffsetFromParent({ 0,0 });
        renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        renderContext.shader.setVec3f("color", glm::vec3(0.0f, 1.0f, 0.0f)); // GREEN
        renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    }

    //HkNode
    void updateMySelf() override
    {
        /*
        I need to reposition myself based on my parent.
        This system implicitly doesn't let you move UI elements who happen to have a parent
        so only top level root objects can be moved.
        The only way to move the children is by offseting them in relation to their parent.
        Note: If this happend to be the selectedNodeId AND it has a parent, because if has a parent
        node will be unable to move on mouse click+drag for exaple because it is constrained to the parent */
        if (const auto& p = getParent().lock())
        {
            transformContext.setPos(p->transformContext.getPos());
        }

        glm::vec2 diff;
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
            // transformContext.setPos(p->transformContext.getPos());
            if (const auto& p = getParent().lock())
            {
                if (sceneDataRef.isMouseClicked && sceneDataRef.maybeFocusedNodeId == getId())
                {
                    diff = {
                        sceneDataRef.mousePos.x - sceneDataRef.lastMousePos.x,
                        sceneDataRef.mousePos.y - sceneDataRef.lastMousePos.y
                    };
                    transformContext.addOffsetFromParent(diff);
                    std::cout << "maybefocused: " << sceneDataRef.maybeFocusedNodeId << " ";
                    std::cout << "currId: " << getId() << "\n";
                    // std::cout << "X diff " << sceneDataRef.mousePos.x - sceneDataRef.lastMousePos.x << " ";
                    // std::cout << "Y diff " << sceneDataRef.mousePos.y - sceneDataRef.lastMousePos.y << "\n";
                }
            }
            else
            {

                /* Safe to assume that this is what dragging the current element logic looks like */
                if (sceneDataRef.isMouseClicked && sceneDataRef.maybeSelectedNodeId == getId())
                {
                    // std::cout << "enters here no parent\n";
                    transformContext.setPos(sceneDataRef.mouseOffsetFromCenter + sceneDataRef.mousePos);
                    // transformContext.setOffsetFromParent(sceneDataRef.mouseOffsetFromCenter);
                }
            }
            break;
        case HkEvent::MouseClick:
            /* If i'm a child container... this should be abstracted into a windowFrame thingy */
            if (const auto& p = getParent().lock())
            {
                std::cout << "enter here\n";
                if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
                {
                    std::cout << "enter here 222\n";

                    sceneDataRef.maybeSelectedNodeId = getId();
                }
            }
            else
            {
                /*
                If mouse is clicked (currently any) and inside UI element bounds, safe to assume this is a
                candidate to be the currently selected node, although it's not guaranteed to be this one. The selected node
                will actually be one of the leaf of the UI tree who hits and validates this check the latest.
                Offset from mouse position to UI node will also be calculated so on mouse move, object doesn't just
                rubber band to center of UI node and instead it keeps a natural offset to it. */
                if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos))
                {
                    sceneDataRef.maybeSelectedNodeId = getId();
                    sceneDataRef.mouseOffsetFromCenter = transformContext.getPos() - sceneDataRef.mousePos;
                }
            }
            break;
        case HkEvent::MouseEnterExit: break;
        case HkEvent::MouseScroll: break;
        case HkEvent::DropPath: break;
        }
        /*Don't forget to show node*/
        renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
        updateChildren();
    }

    void updateChildren()
    {
        for (const auto& child : getChildren())
            child->updateMySelf();
    }

    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren)
    {
        HkNode::pushChildren(newChildren);
    }

    void printTree() { HkNode::printTree(); }

    void setColor(const glm::vec3& color)
    {
        renderContext.shader.setVec3f("color", color);
    }

    void setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

    void setSize(const glm::vec2& size) { transformContext.setScale(size); }

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};


class HkButton : public HkNode
{
public:
    HkButton(const std::string& name)
        : HkNode(name, "Button")
        , sceneDataRef(HkSceneManagement::get().getSceneDataRef())
    {
        if (const auto& p = getParent().lock())
        {
            transformContext.setPos(p->transformContext.getPos());
        }

        transformContext.setOffsetFromParent({ 0,-30 });
        renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        renderContext.shader.setVec3f("color", glm::vec3(1.0f, 0.0f, 0.0f)); // RED
        renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    }

    //HkNode
    void updateMySelf() override
    {
        /*I need to reposition myself based on my parent*/
        if (const auto& p = getParent().lock())
        {
            transformContext.setPos(p->transformContext.getPos());
            transformContext.setOffsetFromParent({ 0,-30 });
        }

        /*main HkEvents handler*/
        switch (sceneDataRef.currentEvent)
        {
        case HkEvent::None: break;
        case HkEvent::GeneralUpdate: break;
        case HkEvent::WindowResize: break;
        case HkEvent::MouseMove:
            break;
        case HkEvent::MouseClick:
            // if (sceneDataRef.isMouseClicked && transformContext.isPosInsideOfNode(sceneDataRef.mousePos)
            //     && sceneDataRef.maybeFocusedNodeId == 9999)
            // {
            //     sceneDataRef.maybeSelectedNodeId = getId();
            //     sceneDataRef.mouseOffsetFromCenter = transformContext.getPos() - sceneDataRef.mousePos;
            // }

            break;
        case HkEvent::MouseEnterExit: break;
        case HkEvent::MouseScroll: break;
        case HkEvent::DropPath: break;
        }
        /*Don't forget to show node*/
        renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    }

    void setPos(const glm::vec2& pos) { transformContext.setPos(pos); }

    void setSize(const glm::vec2& size) { transformContext.setScale(size); }

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;

using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui