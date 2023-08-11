#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string& containerName)
    : HkNodeBase(containerName, "Container")
    , hScrollBar_("{Internal}-HScrollBarFor " + containerName, true)
    , vScrollBar_("{Internal}-VScrollBarFor " + containerName, false)
    , sbCount_{ 0 }
    , isIntersectorNeeded_{ false }
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    /* NOTE: In the future maybe this dummy can be an actual small UI element, but for now let it be
       just a normal renderable detail */
    dummyXYIntersectorData_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    dummyXYIntersectorData_.renderContext.shader.setVec3f("color", glm::vec3(0.7f, 1.0f, 0.2f));
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
    node_.constraintContext.offsetPercentage_.y = vScrollBar_.getScrollValue();

    //TODO: This could be set automatically from ConstraintContext
    node_.constraintContext.hScrollBarWidth_ = hScrollBar_.node_.transformContext.scale.y;
    node_.constraintContext.vScrollBarHeight_ = vScrollBar_.node_.transformContext.scale.x;
}

/* Compute SB constraints only if needed */
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
void HkContainer::handleContainerOverflowIfNeeded()
{
    // hsb
    if (!hScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowX_)
    {
        sbCount_++;
        hScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&hScrollBar_.treeStruct_);
    }
    else if (hScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowX_)
    {
        sbCount_--;
        hScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
    }

    // vsb
    if (!vScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowY_)
    {
        sbCount_++;
        vScrollBar_.setScrollBarActive(true);
        treeStruct_.pushChild(&vScrollBar_.treeStruct_);
    }
    else if (vScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowY_)
    {
        sbCount_--;
        vScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
    }
}

/* Useful to render additional visual non children UI, like XY intersector. This will be called after all children (and sub children)
   of parent have been rendered */
void HkContainer::postChildrenRendered()
{
    /* If both scrollbars are active, it's obvious we need the intersector at bottom right. It is handled in the Container class because
       container should know when both SBs are active and what to do with them. Also clicking on the dummy object basically means clicking
       on the container itself and since coordinates for this location are already known, maybe we can do some particular stuff with that info */
    if (sbCount_ == 2)
    {
        dummyXYIntersectorData_.transformContext.setScale({
            hScrollBar_.node_.transformContext.scale.y,
            hScrollBar_.node_.transformContext.scale.y });

        dummyXYIntersectorData_.transformContext.setPos({
            hScrollBar_.node_.transformContext.pos.x + hScrollBar_.node_.transformContext.scale.x / 2
                + hScrollBar_.node_.transformContext.scale.y / 2,
            hScrollBar_.node_.transformContext.pos.y }
        );
        dummyXYIntersectorData_.renderContext.render(sceneDataRef_.sceneProjMatrix,
            dummyXYIntersectorData_.transformContext.getModelMatrix());
    }
}


void HkContainer::onDrag()
{
    // std::cout << treeStruct_.getName() << " is being dragged now\n";
        //     //TODO: THis logic shall be moved in SM
    // if (!mouseClickPositionSet_)
    // {
    //     clickPosition_ = sceneDataRef_.mousePos;
    //     mouseClickPositionSet_ = true;
    // }

    /* Ignore mouse moves inside intersector area */
    if (sbCount_ == 2 && !dummyXYIntersectorData_.transformContext.isPosInsideOfNode(sceneDataRef_.dragStartMousePosition))
        std::cout << glfwGetTime() << "  " << sceneDataRef_.dragStartMousePosition.x << "  " <<
        sceneDataRef_.dragStartMousePosition.y << "dragging ouside of intersector\n";
}

//TODO: BUG: when click and drag inside intersector area..it does drag, we shall ignore that part when dragging but we dont
void HkContainer::onGeneralMouseMove()
{
    // /* Safe to assume that this is what dragging the current element logic looks like */
    // //TODO: A dragging state needs to be added in SM => added, just adapt THIS code

    // if (sceneDataRef_.isMouseClicked && sceneDataRef_.focusedId == treeStruct_.getId())
    // {
    //     //TODO: THis logic shall be moved in SM
    //     if (!mouseClickPositionSet_)
    //     {
    //         clickPosition_ = sceneDataRef_.mousePos;
    //         mouseClickPositionSet_ = true;
    //     }

    //     /* Ignore mouse moves inside intersector area */
    //     if (sbCount_ == 2 && !dummyXYIntersectorData_.transformContext.isPosInsideOfNode(clickPosition_))
    //         std::cout << glfwGetTime() << "dragging on container with 2 SBs\n";
    // }
}

void HkContainer::onGeneralMouseClick()
{
    if (!sceneDataRef_.isMouseClicked)
    {
        mouseClickPositionSet_ = false;
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

//TODO: bellow to be removed later when constraints come
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