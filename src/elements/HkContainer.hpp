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
        transformContext.setOffsetFromParent({ 0,0 });
        renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        renderContext.shader.setVec3f("color", glm::vec3(0.0f, 1.0f, 0.0f)); // GREEN
        renderContext.render(sceneDataRef.sceneProjMatrix, transformContext.getModelMatrix());
    }
    glm::vec2 diff{ 0,0 };

    //HkNode
    void updateMySelf() override
    {
        /*main HkEvents handler*/
        switch (sceneDataRef.currentEvent)
        {
        case HkEvent::None: break;
        case HkEvent::GeneralUpdate: break;
        case HkEvent::WindowResize:
            if (transformContext.getPos().x > sceneDataRef.windowWidth)
            {
                transformContext.setPos({ sceneDataRef.windowWidth, transformContext.getPos().y });
            }
            break;
        case HkEvent::MouseMove:
            if (sceneDataRef.maybeSelectedNodeId == getId() && diff == glm::vec2{ 0, 0 })
            {
                diff = transformContext.getPos() - glm::vec2{ sceneDataRef.mousePosX, sceneDataRef.mousePosY };
                std::cout << "calculating " << diff.x << " " << diff.y << '\n';
            }

            if (sceneDataRef.isMouseClicked && sceneDataRef.maybeSelectedNodeId == getId())
            {
                transformContext.setPos(glm::vec2{ sceneDataRef.mousePosX, sceneDataRef.mousePosY } + diff);
            }
            break;
        case HkEvent::MouseClick:
            if (!sceneDataRef.isMouseClicked)
            {
                diff = { 0,0 };
            }

            if (!sceneDataRef.isMouseClicked
                && transformContext.isPosInsideOfNode({ sceneDataRef.mousePosX, sceneDataRef.mousePosY }))
            {
                sceneDataRef.maybeSelectedNodeId = getId();
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
            if (!sceneDataRef.isMouseClicked
                && transformContext.isPosInsideOfNode({ sceneDataRef.mousePosX, sceneDataRef.mousePosY }))
            {
                sceneDataRef.maybeSelectedNodeId = getId();
            }

            // if (!sceneDataRef.isMouseClicked)
            // {
            //     const auto p = getParent().lock(); // this could be cached if used a lot
            //     if (p)
            //     {
            //         // std::cout << "My parent as a " << this << " is " << p << '\n';
            //         p->transformContext.setPos({ sceneDataRef.mousePosX, sceneDataRef.mousePosY });
            //     }
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