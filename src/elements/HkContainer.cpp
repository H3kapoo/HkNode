#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string& containerName)
    : HkNodeBase(containerName, "Container")
    , hScrollBar_("{Internal}-HScrollBarFor " + containerName, true)
    , vScrollBar_("{Internal}-VScrollBarFor " + containerName, false)
    , sbCount_{ 0 }
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

//TODO: Just for experimenting
void HkContainer::scrollbars(bool x, bool y)
{
    x ? treeStruct_.pushChild(&hScrollBar_.treeStruct_) : treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
    y ? treeStruct_.pushChild(&vScrollBar_.treeStruct_) : treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
    sbCount_ = x + y;
}

void HkContainer::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    HkNodeBase::resolveConstraints(children);

    node_.constraintContext.scrollBarConstrain(hScrollBar_.node_.transformContext, true);
    node_.constraintContext.scrollBarConstrain(vScrollBar_.node_.transformContext, false);
}

void HkContainer::onGeneralUpdate()
{
}

void HkContainer::onGeneralMouseMove()
{}

void HkContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        /* What this does is basically push any new children before the scrollbars children so that the
        scrollbars will always be rendered last */
        if (sbCount_)
        {
            const auto it = treeStruct_.getChildren().begin() + treeStruct_.getChildren().size() - sbCount_;
            treeStruct_.pushChildAfter(it, &child->treeStruct_);
        }
        /* If no scrollbars, push normally */
        else
        {
            treeStruct_.pushChild(&child->treeStruct_);
        }
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