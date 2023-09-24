#include "HkContainer.hpp"

namespace hkui
{
HkContainer::HkContainer(const std::string& containerName)
    : HkNodeBase(containerName, HkNodeType::Container)
    , hScrollBar_("{Internal}-HScrollBarFor " + containerName, true)
    , vScrollBar_("{Internal}-VScrollBarFor " + containerName, false)
    , scrollbBarsCount_{ 0 }
{
    node_.styleContext.setColor(glm::vec3(0.5f, 0.5f, 0.5f));

    /* NOTE: In the future maybe this dummy can be an actual small UI element, but for now let it be
       just a normal renderable detail */
    dummyXYIntersectorData_.renderContext.injectStyleContext(&dummyXYIntersectorData_.styleContext);
    dummyXYIntersectorData_.renderContext.setColorUniformEnabled(true);
    dummyXYIntersectorData_.styleContext.setColor(glm::vec3(0.7f, 1.0f, 0.2f));

    //TODO: Dummy just to test nested scrollbars
    if (treeStruct_.getName() == "MyContainer2")
    {
        vScrollBar_.node_.styleContext.setColor(glm::vec3(0.4f, 0.2f, 0.6f));
        hScrollBar_.node_.styleContext.setColor(glm::vec3(0.4f, 0.2f, 0.6f));
    }
}

void HkContainer::onFirstHeartbeat()
{
    node_.renderContext.setShaderSource(
        "assets/shaders/v1.glsl",
        "assets/shaders/f1.glsl",
        &windowDataPtr_->renderStore);
    dummyXYIntersectorData_.renderContext.setShaderSource(
        "assets/shaders/v1.glsl",
        "assets/shaders/f1.glsl",
        &windowDataPtr_->renderStore);
}

//TODO: The way we handle scroll inside scroll is now necessarly very intuitive. It needs to ve changed in the future
void HkContainer::onScroll()
{
    if (vScrollBar_.isScrollBarActive())
    {
        vScrollBar_.onScroll();
    }
}

void HkContainer::onGeneralMouseScroll()
{
    if (windowDataPtr_->nearestScrollContainerId_ == treeStruct_.getId())
    {
        vScrollBar_.onScroll();
        //TODO: Allow H movement but only if shift key or ctrl key is pressed as well
        // hScrollBar_.onScroll();
    }
}

void HkContainer::onGeneralMouseClick()
{
    //TODO: Later bring back highlighting if needed
    // node_.renderContext.getShader().setInt("focused", 0);
}

void HkContainer::onGeneralMouseMove()
{
    // //TODO: Later bring back highlighting if needed
    // if (sceneDataRef_.hoveredId == treeStruct_.getId())
    // {
    //     // node_.renderContext.getShader().setVec3f("hovered", glm::vec3(1, 1, 1));
    //     node_.renderContext.getShader().setInt("hovered", 1);
    // }
    // else
    // {
    //     node_.renderContext.getShader().setInt("hovered", 0);
    //     //     // node_.renderContext.getShader().setVec3f("hovered", glm::vec3(0, 1, 1));
    // }
}

void HkContainer::onClick()
{
    // node_.renderContext.getShader().setInt("focused", 1);
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

void HkContainer::resolveChildrenConstraints(HkTreeStruct& children,
    const HkScrollbarsSize&)
{
    /* We need to notify constraint ctx about scrollbars scroll value so we can offset the children if needed*/
    //TODO: OPtimize: why do this even if we dont have SBs?
    node_.constraintContext.offsetPercentage_.x = hScrollBar_.getScrollValue();
    node_.constraintContext.offsetPercentage_.y = vScrollBar_.getScrollValue();

    /* Resolve children constraints (ignores scrollbar children) */
    //TODO: This ^ resolve shall be done base on node constrain policy, add function here too for setting policy
    HkNodeBase::resolveChildrenConstraints(children,
        {
    hScrollBar_.node_.transformContext.getScale().y,
    vScrollBar_.node_.transformContext.getScale().x
        });

    // // /* Reolve contraints but this time only for scrollbars, if needed */
    resolveScrollBarChildrenIfNeeded();
}

/* Useful to render additional visual non children UI, like XY intersector. This will be called after all children (and sub children)
   of parent have been rendered */
void HkContainer::postRenderAdditionalDetails()
{
    /* If both scrollbars are active, it's obvious we need the intersector at bottom right. It is handled in the Container class because
       container should know when both SBs are active and what to do with them. Also clicking on the dummy object basically means clicking
       on the container itself and since coordinates for this location are already known, maybe we can do some particular stuff with that info */
       // if (scrollbBarsCount_ == 2)
    {
        // if (treeStruct_.getId() == 4) return;
        // dummyXYIntersectorData_.transformContext.setScale({
        //     hScrollBar_.node_.transformContext.getScale().y,
        //     hScrollBar_.node_.transformContext.getScale().y });

        // // dummyXYIntersectorData_.transformContext.setPos({
        // //     hScrollBar_.node_.transformContext.getPos().x + hScrollBar_.node_.transformContext.getScale().x,
        // //     hScrollBar_.node_.transformContext.getPos().y });

        // dummyXYIntersectorData_.transformContext.setPos({
        //     node_.transformContext.getPos().x + node_.transformContext.getScale().x - 4,
        //     node_.transformContext.getPos().y + node_.transformContext.getScale().y - 4 });

        // dummyXYIntersectorData_.renderContext.render(sceneDataRef_.sceneProjMatrix,
        //     dummyXYIntersectorData_.transformContext.getModelMatrix());

        // dummyXYIntersectorData_.transformContext.setPos({
        //     node_.transformContext.getPos().x,
        //     node_.transformContext.getPos().y });

        // dummyXYIntersectorData_.renderContext.render(sceneDataRef_.sceneProjMatrix,
        //     dummyXYIntersectorData_.transformContext.getModelMatrix());

    }
}

/* This determines if the container will have a scrollbar showing or not */
void HkContainer::resolveScrollBarChildrenIfNeeded()
{
    /* Place horizontal scrollbar if neeeded */
    if (node_.styleContext.isOverflowAllowedX())
    {
        hScrollBar_.setOverflowSize(node_.constraintContext.overflowXYSize_.x);
        if (!hScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowX_)
        {
            scrollbBarsCount_++; //TODO: THis variable is now in constraint context
            treeStruct_.pushChild(&hScrollBar_.treeStruct_);
            hScrollBar_.injectWindowDataPtr(windowDataPtr_); //NOTE: Not really needed each time
            hScrollBar_.knob_.injectWindowDataPtr(windowDataPtr_);
            hScrollBar_.setScrollBarActive(true);
        }
        else if (hScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowX_)
        {
            scrollbBarsCount_--;
            hScrollBar_.setScrollBarActive(false);
            treeStruct_.removeChildren({ hScrollBar_.treeStruct_.getId() });
        }
        node_.constraintContext.scrollBarConstrain(hScrollBar_.node_.transformContext);
    }

    /* Place vertical scrollbar if neeeded */
    if (node_.styleContext.isOverflowAllowedY())
    {
        vScrollBar_.setOverflowSize(node_.constraintContext.overflowXYSize_.y);
        if (!vScrollBar_.isScrollBarActive() && node_.constraintContext.isOverflowY_)
        {
            scrollbBarsCount_++;
            treeStruct_.pushChild(&vScrollBar_.treeStruct_);
            //TODO: There has to be a better way
            vScrollBar_.injectWindowDataPtr(windowDataPtr_);
            vScrollBar_.knob_.injectWindowDataPtr(windowDataPtr_);
            vScrollBar_.setScrollBarActive(true);
        }
        else if (vScrollBar_.isScrollBarActive() && !node_.constraintContext.isOverflowY_)
        {
            scrollbBarsCount_--;
            vScrollBar_.setScrollBarActive(false);
            treeStruct_.removeChildren({ vScrollBar_.treeStruct_.getId() });
        }
        node_.constraintContext.scrollBarConstrain(vScrollBar_.node_.transformContext);

        //TODO: Note? somehow if we put these inside the ifs above, altough flow enters in IFs only one time,
        // in runtime, scrollbars will still constrain each frame somehow, but bellow functions arent called..
        // something to do with references ? Not really a but but worth noting.
    }
}

void HkContainer::pushChildren(const std::vector<HkNodeBasePtr>& newChildren)
{
    for (const auto& child : newChildren)
    {
        /* Inject ref to windowData to each children so they all have a common source of truth*/
        child->injectWindowDataPtr(windowDataPtr_);

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
            child->injectWindowDataPtr(windowDataPtr_);
        }
    }
}

void HkContainer::setSize(const glm::vec2& size)
{
    node_.transformContext.setScale(size);
}
} // hkui