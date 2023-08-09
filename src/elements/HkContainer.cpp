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

void HkContainer::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /* Resolve children constraints (ignores scrollbar children) */
    HkNodeBase::resolveConstraints(children);

    /* Exactly what it says, show bars or not if overflow occured*/
    handleContainerOverflowIfNeeded();

    /* Resolve scrollbar only constraints for axis */
    constrainScrollbarsIfNeeded();

    //Not sure about this one here..
    node_.constraintContext.offsetPercentage_.x = hScrollBar_.getScrollValue();

    //TODO: This could be set automatically from ConstraintContext
    node_.constraintContext.hScrollBarWidth_ = hScrollBar_.node_.transformContext.scale.y;
    node_.constraintContext.vScrollBarHeight_ = vScrollBar_.node_.transformContext.scale.x;
}

/* Compute SB constraints only if needed */
void HkContainer::constrainScrollbarsIfNeeded()
{
    if (node_.constraintContext.isOverflowX)
    {
        node_.constraintContext.scrollBarConstrain(hScrollBar_.node_.transformContext, true);
    }
    if (node_.constraintContext.isOverflowY)
    {
        node_.constraintContext.scrollBarConstrain(vScrollBar_.node_.transformContext, false);
    }
}


/* This determines if the container will have a scrollbar showing or not */
void HkContainer::handleContainerOverflowIfNeeded()
{
    // hsb
    if (!hScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowX)
    {
        sbCount_++;
        hScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&hScrollBar_.treeStruct_);
    }
    else if (hScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowX)
    {
        sbCount_--;
        hScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
    }

    // vsb
    if (!vScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowY)
    {
        sbCount_++;
        vScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&vScrollBar_.treeStruct_);
    }
    else if (vScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowY)
    {
        sbCount_--;
        vScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
    }
}

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