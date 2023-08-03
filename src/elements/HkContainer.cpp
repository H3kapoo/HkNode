#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string& containerName)
    : HkNodeBase(containerName, "Container")
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

void HkContainer::onGeneralUpdate()
{
    //TODO: this should definitely dissapear from here and a more efficent way needs to be implemented
    //      . experimental for now
    std::vector<HkTransformContext*> childTcs;
    childTcs.reserve(treeStruct_.getChildren().size());
    for (const auto& child : treeStruct_.getChildren())
    {
        childTcs.push_back(&child->getPayload()->node_.transformContext);
    }

    node_.constraintContext.freeConstraint(
        node_.transformContext,
        childTcs
    );
}

void HkContainer::onGeneralMouseMove()
{}

void HkContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        treeStruct_.pushChild(&child->treeStruct_);
    }
}

// bewllow to be removed later when constraints come
void HkContainer::setColor(const glm::vec3& color)
{
    node_.renderContext.shader.setVec3f("color", color);
}

void HkContainer::setPos(const glm::vec2& pos) {
    node_.transformContext.setPos(pos);
}

void HkContainer::setSize(const glm::vec2& size) {
    node_.transformContext.setScale(size);
}
} // hkui