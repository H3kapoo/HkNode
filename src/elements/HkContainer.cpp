#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string& containerName)
    : HkNodeBase(containerName, "Container")
    , hScrollBar_("{Internal}-HScrollBarFor " + containerName, true)
    , vScrollBar_("{Internal}-VScrollBarFor " + containerName, false)
    , scrollbBarsCount_{ 0 }
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    /* NOTE: In the future maybe this dummy can be an actual small UI element, but for now let it be
       just a normal renderable detail */
    dummyXYIntersectorData_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    dummyXYIntersectorData_.renderContext.shader.setVec3f("color", glm::vec3(0.7f, 1.0f, 0.2f));

    //Dummy just to test nested scrollbars
    if (treeStruct_.getName() == "MyContainer2")
    {
        vScrollBar_.node_.renderContext.shader.setVec3f("color", glm::vec3(0.4f, 0.2f, 0.6f));
        hScrollBar_.node_.renderContext.shader.setVec3f("color", glm::vec3(0.4f, 0.2f, 0.6f));
    }
}

//TODO: Disable scrollbars outside of view area
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
    node_.constraintContext.offsetPercentage_.y = vScrollBar_.getScrollValue();

    //TODO: This could be set automatically from ConstraintContext
    node_.constraintContext.hScrollBarWidth_ = hScrollBar_.node_.transformContext.scale.y;
    node_.constraintContext.vScrollBarHeight_ = vScrollBar_.node_.transformContext.scale.x;
}

/* Compute SB constraints only if needed */
//TODO: This could be moved directly inside the constraint context
void HkContainer::constrainScrollbarsIfNeeded()
{
    if (node_.constraintContext.isOverflowX_)
    {
        node_.constraintContext.scrollBarConstrain(hScrollBar_.node_.transformContext, true);
    }
    if (node_.constraintContext.isOverflowY_)
    {
        node_.constraintContext.scrollBarConstrain(vScrollBar_.node_.transformContext, false);
    }
}

/* This determines if the container will have a scrollbar showing or not */
//TODO: This could be moved directly inside the constraint context
void HkContainer::handleContainerOverflowIfNeeded()
{
    //TODO: Maybe just turn them inactive instead of removing them?
    // hsb
    if (!hScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowX_)
    {
        scrollbBarsCount_++;
        hScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&hScrollBar_.treeStruct_);
    }
    else if (hScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowX_)
    {
        scrollbBarsCount_--;
        hScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
    }

    // vsb
    if (!vScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowY_)
    {
        scrollbBarsCount_++;
        vScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&vScrollBar_.treeStruct_);
    }
    else if (vScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowY_)
    {
        scrollbBarsCount_--;
        vScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
    }
}

/* Useful to render additional visual non children UI, like XY intersector. This will be called after all children (and sub children)
   of parent have been rendered */
void HkContainer::preRenderAdditionalDetails()
{
    /* If both scrollbars are active, it's obvious we need the intersector at bottom right. It is handled in the Container class because
       container should know when both SBs are active and what to do with them. Also clicking on the dummy object basically means clicking
       on the container itself and since coordinates for this location are already known, maybe we can do some particular stuff with that info */
    if (scrollbBarsCount_ == 2)
    {
        dummyXYIntersectorData_.transformContext.setScale({
            hScrollBar_.node_.transformContext.scale.y,
            hScrollBar_.node_.transformContext.scale.y });

        dummyXYIntersectorData_.transformContext.setPos({
            hScrollBar_.node_.transformContext.pos.x + hScrollBar_.node_.transformContext.scale.x,
            hScrollBar_.node_.transformContext.pos.y });

        dummyXYIntersectorData_.renderContext.render(sceneDataRef_.sceneProjMatrix,
            dummyXYIntersectorData_.transformContext.getModelMatrix());
    }
}

void HkContainer::onDrag()
{
    // node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromFocusedCenter + sceneDataRef_.mousePos);

    //TODO: Should this really be here?
    /* Ignore mouse drags inside intersector area */
    if (scrollbBarsCount_ == 2 && !dummyXYIntersectorData_.transformContext.isPosInsideOfNodeViewableArea(sceneDataRef_.dragStartMousePosition))
        std::cout << glfwGetTime() << "  " << sceneDataRef_.dragStartMousePosition.x << "  " <<
        sceneDataRef_.dragStartMousePosition.y << "dragging ouside of intersector\n";
}

void HkContainer::onGeneralMouseMove()
{}

void HkContainer::onGeneralMouseClick()
{
}

void HkContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        /* What this does is basically push any new children before the scrollbars children so that the
        scrollbars will always be rendered last */
        if (scrollbBarsCount_)
        {
            const auto it = treeStruct_.getChildren().begin() + treeStruct_.getChildren().size() - scrollbBarsCount_;
            treeStruct_.pushChildAfter(it, &child->treeStruct_);
        }
        /* If no scrollbars, push normally */
        else
        {
            treeStruct_.pushChild(&child->treeStruct_);
        }
    }
}

//TODO: bellow to be removed later when constraints come
void HkContainer::setColor(const glm::vec3& color)
{
    node_.renderContext.shader.setVec3f("color", color);
}

void HkContainer::setPos(const glm::vec2& pos)
{
    node_.transformContext.setPos(pos);
}

void HkContainer::setSize(const glm::vec2& size)
{
    node_.transformContext.setScale(size);
}
} // hkui