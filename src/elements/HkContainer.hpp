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
        , scenDataRef(HkSceneManagement::get().sceneData)
    {
        renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        renderContext.shader.setVec3f("color", glm::vec3(0.0f, 1.0f, 0.0f)); // GREEN
        renderContext.render(transformContext.getModelMatrix());
    }

    //HkNode
    void updateMySelf() override
    {
        /*main HkEvents handler*/
        switch (scenDataRef.currentEvent)
        {
        case HkEvent::None:
        case HkEvent::GeneralUpdate: break;
        case HkEvent::WindowResize:
        case HkEvent::MouseMove: break;
        case HkEvent::MouseClick:
            if (!scenDataRef.isMouseClicked)
            {
                // renderContext.setPos({ scenDataRef.mousePosX, scenDataRef.mousePosY });
            }
            break;
        case HkEvent::MouseEnterExit:
        case HkEvent::MouseScroll:
        case HkEvent::DropPath:
            break;
        }
        renderContext.render(transformContext.getModelMatrix());
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

private:
    HkSceneData& scenDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};


class HkButton : public HkNode
{
public:
    HkButton(const std::string& name)
        : HkNode(name, "Button")
        , scenDataRef(HkSceneManagement::get().sceneData)
    {
        renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        renderContext.shader.setVec3f("color", glm::vec3(1.0f, 0.0f, 0.0f)); // RED
        renderContext.render(transformContext.getModelMatrix());
    }

    //HkNode
    void updateMySelf() override
    {
        /*main HkEvents handler*/
        switch (scenDataRef.currentEvent)
        {
        case HkEvent::None:
        case HkEvent::GeneralUpdate: break;
        case HkEvent::WindowResize:
        case HkEvent::MouseMove:
            break;
        case HkEvent::MouseClick:
            if (!scenDataRef.isMouseClicked)
            {
                const auto p = getParent().lock(); // this could be cached if used a lot
                if (p)
                {
                    std::cout << "My parent as a " << this << " is " << p << '\n';
                    p->transformContext.setPos({ scenDataRef.mousePosX, scenDataRef.mousePosY });
                }
            }
            break;
        case HkEvent::MouseEnterExit:
        case HkEvent::MouseScroll:
        case HkEvent::DropPath:
            break;
        }
        /*Don't forget to only show node*/
        renderContext.render(transformContext.getModelMatrix());
    }
private:
    HkSceneData& scenDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;

using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui