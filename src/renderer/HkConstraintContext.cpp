#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"

namespace hkui
{

void HkConstraintContext::setRootTc(HkTransformContext* rootTc)
{
    thisTc_ = rootTc;
}

void HkConstraintContext::setPolicy(const HkConstraintPolicy policy) { policy_ = policy; }

void HkConstraintContext::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    if (children.empty()) return;

    switch (policy_)
    {
    case HkConstraintPolicy::AlignHorizontally:
        alignHorizontally(children);
        break;
    }
}

void HkConstraintContext::alignHorizontally(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    const auto& pos = thisTc_->pos;
    const auto& size = thisTc_->scale;

    const auto childrenWidth = std::accumulate(children.begin(), children.end(), 0,
        [](int total, HkTreeStructure<HkNodeBase>* child)
        {
            return total + child->getPayload()->node_.transformContext.scale.x;
        });

    const auto spaceLeft = size.x - childrenWidth;

    const auto slotSize = size.x / children.size();

    auto& firstChildTc = children.at(0)->getPayload()->node_.transformContext;
    auto startPosX = pos.x - size.x / 2 + spaceLeft / 2;
    for (const auto& child : children)
    {
        auto& childTc = child->getPayload()->node_.transformContext;
        startPosX += childTc.scale.x / 2;
        childTc.setPos({ startPosX , pos.y });
        startPosX += childTc.scale.x / 2;
    }
}

void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& childTc)
{
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    childTc.setPos({ thisTc_->pos.x, thisTc_->pos.y + childTc.scale.y / 2 + thisTc_->scale.y / 2 });
}

void HkConstraintContext::freeConstraint(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    if (children.empty()) return;
    const auto& pos = thisTc_->pos;
    const auto& size = thisTc_->scale;

    // const auto childLen = std::accumulate(childTcs.begin(), childTcs.end(), 0,
        // [](int total, HkTransformContext* child) {return total + child->scale.x;});

    // const auto spaceLeft = size.x - childLen;

    uint32_t i = 0;
    const auto slotSize = size.x / children.size();
    const auto startXPos = pos.x - size.x / 2 + slotSize / 2; // (1) slot even spacing
    // auto startXPos = pos.x - size.x / 2;
    for (const auto& child : children)
    {
        // (void)childTc;
        auto& childTc = child->getPayload()->node_.transformContext;
        childTc.setPos({ startXPos + i * (slotSize), pos.y }); // (1) slot even spacing
        // childTc->setPos({ startXPos, pos.y });
        // tc_.setPos({ startXPos, pos.y });
        // rc_.render(HkSceneManagement::get().getSceneDataRef().sceneProjMatrix, tc_.getModelMatrix());

        // startXPos += childTc->scale.x;
        i++;
    }

}
} // hkui