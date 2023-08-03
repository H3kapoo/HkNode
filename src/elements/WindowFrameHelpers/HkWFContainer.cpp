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

void HkWFContainer::onGeneralUpdate()
{
    repositionBasedOnParent();
}

void HkWFContainer::onGeneralMouseMove()
{
}

void HkWFContainer::repositionBasedOnParent()
{
    /* This node type is always guaranteed to have a parent */
    HkNodeBase* parent = treeStruct_.getParent()->getPayload();
    const auto pos = parent->node_.transformContext.getPos();
    const auto scale = parent->node_.transformContext.getScale();
    const auto thisScale = node_.transformContext.getScale();

    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    node_.transformContext.setPos({ pos.x, pos.y + thisScale.y / 2 + scale.y / 2 });
    // node_.transformContext.setScale({ scale.x, 200 });
}

void HkWFContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        treeStruct_.pushChild(&child->treeStruct_);
    }
}
} // hkui