#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"

namespace hkui
{

void HkConstraintContext::setRootTc(HkTransformContext* rootTc) { thisTc_ = rootTc; }

void HkConstraintContext::setPolicy(const HkConstraintPolicy policy) { policy_ = policy; }

void HkConstraintContext::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    if (children.empty()) return;

    /* TODO: dirty flags shall be used here to not do redundant repositioning */

    switch (policy_)
    {
    case HkConstraintPolicy::AlignHorizontally:
        alignHorizontally(children);
        break;
    }
}

void HkConstraintContext::alignVertically(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Centers children vertically inside the parent and places children one after the other vertically.
        Parent's X value is used for the children X value.
    */
    const auto childrenHeight = std::accumulate(children.begin(), children.end(), 0,
        [](int total, HkTreeStructure<HkNodeBase>* child)
        {
            return total + child->getPayload()->node_.transformContext.scale.y;
        });

    const auto spaceLeft = thisTc_->scale.y - childrenHeight;

    auto startPosY = thisTc_->pos.y - thisTc_->scale.y / 2 + spaceLeft / 2;
    for (const auto& child : children)
    {
        auto& childTc = child->getPayload()->node_.transformContext;
        startPosY += childTc.scale.y / 2;
        childTc.setPos({ thisTc_->pos.x, startPosY });
        startPosY += childTc.scale.y / 2;
    }
}

void HkConstraintContext::alignHorizontally(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Centers children horizontally inside the parent and places children one after the other horizontally.
        Parent's Y value is used for the children Y value.
    */
    const auto childrenWidth = std::accumulate(children.begin(), children.end(), 0,
        [](int total, HkTreeStructure<HkNodeBase>* child)
        {
            return total + child->getPayload()->node_.transformContext.scale.x;
        });

    const auto spaceLeft = thisTc_->scale.x - childrenWidth;

    auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + spaceLeft / 2;
    for (const auto& child : children)
    {
        auto& childTc = child->getPayload()->node_.transformContext;
        startPosX += childTc.scale.x / 2;
        childTc.setPos({ startPosX , thisTc_->pos.y });
        startPosX += childTc.scale.x / 2;
    }
}

void HkConstraintContext::scrollBarConstrain(const bool isHorizontalBar)
{

}

void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& childTc)
{
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    childTc.setPos({ thisTc_->pos.x, thisTc_->pos.y + childTc.scale.y / 2 + thisTc_->scale.y / 2 });
}

void HkConstraintContext::freeConstraint(const std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    /*
        Centers children horizontally inside the parent and places children one after the other horizontally.
        Parent's Y value is used for the children Y value.
    */
    // const auto childrenWidth = std::accumulate(children.begin(), children.end(), 0,
    //     [](int total, HkTreeStructure<HkNodeBase>* child)
    //     {
    //         return total + child->getPayload()->node_.transformContext.scale.x;
    //     });

    // const auto spaceLeft = thisTc_->scale.x - childrenWidth;

    // auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + spaceLeft / 2;
    // for (const auto& child : children)
    // {
    //     auto& childTc = child->getPayload()->node_.transformContext;
    //     startPosX += childTc.scale.x / 2;
    //     childTc.setPos({ startPosX , thisTc_->pos.y });
    //     startPosX += childTc.scale.x / 2;
    // }
    // ---------------------------------------------------------------------------------------------------------
    // /*
    //     Centers children vertically inside the parent and places children one after the other vertically.
    //     Parent's X value is used for the children X value.
    // */
    // const auto childrenHeight = std::accumulate(children.begin(), children.end(), 0,
    //     [](int total, HkTreeStructure<HkNodeBase>* child)
    //     {
    //         return total + child->getPayload()->node_.transformContext.scale.y;
    //     });

    // const auto spaceLeft = thisTc_->scale.y - childrenHeight;

    // auto startPosY = thisTc_->pos.y - thisTc_->scale.y / 2 + spaceLeft / 2;
    // for (const auto& child : children)
    // {
    //     auto& childTc = child->getPayload()->node_.transformContext;
    //     startPosY += childTc.scale.y / 2;
    //     childTc.setPos({ thisTc_->pos.x, startPosY });
    //     startPosY += childTc.scale.y / 2;
    // }
}
} // hkui