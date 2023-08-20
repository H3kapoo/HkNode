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
    node_.renderContext.getShader().setVec3f("color", glm::vec3(0.5f, 0.5f, 0.5f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    /* NOTE: In the future maybe this dummy can be an actual small UI element, but for now let it be
       just a normal renderable detail */
    dummyXYIntersectorData_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    dummyXYIntersectorData_.renderContext.getShader().setVec3f("color", glm::vec3(0.7f, 1.0f, 0.2f));

    //TODO: Dummy just to test nested scrollbars
    if (treeStruct_.getName() == "MyContainer2")
    {
        vScrollBar_.node_.renderContext.getShader().setVec3f("color", glm::vec3(0.4f, 0.2f, 0.6f));
        hScrollBar_.node_.renderContext.getShader().setVec3f("color", glm::vec3(0.4f, 0.2f, 0.6f));
    }
}

void HkContainer::onDrag()
{
    // node_.transformContext.setPos(sceneDataRef_.mouseOffsetFromFocusedCenter + sceneDataRef_.mousePos);

    //TODO: Should this really be here?
    //TODO: We cant realy use inside viewable area since it doesnt have a parent..
    /* Ignore mouse drags inside intersector area */
    if (scrollbBarsCount_ == 2 && dummyXYIntersectorData_.transformContext.isPosInsideOfNode(sceneDataRef_.dragStartMousePosition))
    {
    }
    else
    {
        // std::cout << glfwGetTime() << "  " << sceneDataRef_.dragStartMousePosition.x << "  " <<
            // sceneDataRef_.dragStartMousePosition.y << "dragging ouside of intersector\n";
    }
}

void HkContainer::resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /* We need to notify constraint ctx about scrollbars scroll value so we can offset the children if needed*/
    node_.constraintContext.offsetPercentage_.x = hScrollBar_.getScrollValue();
    node_.constraintContext.offsetPercentage_.y = vScrollBar_.getScrollValue();

    /* Resolve children constraints (ignores scrollbar children) */
    //TODO: This ^ resolve shall be done base on node constrain policy, add function here too for setting policy
    HkNodeBase::resolveChildrenConstraints(children);

    /* Reolve contraints but this time only for scrollbars, if needed */
    resolveScrollBarChildrenIfNeeded();
}

/* Useful to render additional visual non children UI, like XY intersector. This will be called after all children (and sub children)
   of parent have been rendered */
void HkContainer::postRenderAdditionalDetails()
{
    /* If both scrollbars are active, it's obvious we need the intersector at bottom right. It is handled in the Container class because
       container should know when both SBs are active and what to do with them. Also clicking on the dummy object basically means clicking
       on the container itself and since coordinates for this location are already known, maybe we can do some particular stuff with that info */
    if (scrollbBarsCount_ == 2)
    {
        dummyXYIntersectorData_.transformContext.setScale({
            hScrollBar_.node_.transformContext.getScale().y,
            hScrollBar_.node_.transformContext.getScale().y });

        dummyXYIntersectorData_.transformContext.setPos({
            hScrollBar_.node_.transformContext.getPos().x + hScrollBar_.node_.transformContext.getScale().x,
            hScrollBar_.node_.transformContext.getPos().y });

        dummyXYIntersectorData_.renderContext.render(sceneDataRef_.sceneProjMatrix,
            dummyXYIntersectorData_.transformContext.getModelMatrix());
    }
}

/* This determines if the container will have a scrollbar showing or not */
void HkContainer::resolveScrollBarChildrenIfNeeded()
{
    /* Place horizontal scrollbar if neeeded */
    hScrollBar_.setOverflowSize(node_.constraintContext.overflowXYSize_.x);
    if (!hScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowX_)
    {
        scrollbBarsCount_++;
        treeStruct_.pushChild(&hScrollBar_.treeStruct_);
        hScrollBar_.setScrollBarActive(true);
    }
    else if (hScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowX_)
    {
        scrollbBarsCount_--;
        hScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
    }

    /* Place vertical scrollbar if neeeded */
    vScrollBar_.setOverflowSize(node_.constraintContext.overflowXYSize_.y);
    if (!vScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowY_)
    {
        scrollbBarsCount_++;
        treeStruct_.pushChild(&vScrollBar_.treeStruct_);
        vScrollBar_.setScrollBarActive(true);
    }
    else if (vScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowY_)
    {
        scrollbBarsCount_--;
        vScrollBar_.setScrollBarActive(false);
        treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
    }

    //TODO: Note? somehow if we put these inside the ifs above, altough flow enters in IFs only one time,
    // in runtime, scrollbars will still constrain each frame somehow, but bellow functions arent called..
    // something to do with references ? Not really a but but worth noting.
    node_.constraintContext.scrollBarConstrain(vScrollBar_.node_.transformContext);
    node_.constraintContext.scrollBarConstrain(hScrollBar_.node_.transformContext);
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

//TODO: bellow to be refactored later when style comes
void HkContainer::setColor(const glm::vec3& color)
{
    node_.renderContext.getShader().setVec3f("color", color);
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