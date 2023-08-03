#pragma once

#include <vector>
#include <numeric>

#include "HkTransformContext.hpp"
#include "HkRenderContext.hpp"

namespace hkui
{
class HkConstraintContext
{
public:
    HkRenderContext rc_;
    HkTransformContext tc_;
    HkConstraintContext()
    {
        rc_.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        rc_.shader.setVec3f("color", glm::vec3(1.0f, 0.2f, 0.9f)); // BLUEish
        rc_.render(HkSceneManagement::get().getSceneDataRef().sceneProjMatrix, tc_.getModelMatrix());
        tc_.setScale({ 20,400 });
    }

    void windowFrameContainerConstraint(HkTransformContext& parentTc, HkTransformContext& childTc)
    {
        const auto& pos = parentTc.getPos();
        const auto& scale = parentTc.getScale();
        const auto& childScale = childTc.getScale();

        /* TODO: dirty flags shall be used here to not do redundant repositioning */
        childTc.setPos({ pos.x, pos.y + childScale.y / 2 + scale.y / 2 });
        // node_.transformContext.setScale({ scale.x, 200 });
    }

    void freeConstraint(HkTransformContext& parentTc, const std::vector<HkTransformContext*>& childTcs)
    {
        if (childTcs.empty()) return;
        const auto& pos = parentTc.getPos();
        const auto& size = parentTc.getScale();

        const auto childLen = std::accumulate(childTcs.begin(), childTcs.end(), 0,
            [](int total, HkTransformContext* child) {return total + child->getScale().x;});

        const auto spaceLeft = size.x - childLen;

        uint32_t i = 0;
        // const auto slotSize = size.x / childTcs.size();
        // const auto startXPos = pos.x - size.x / 2 + slotSize / 2; // (1) slot even spacing
        auto startXPos = pos.x - size.x / 2;
        for (const auto& childTc : childTcs)
        {
            // (void)childTc;
            // childTc->setPos({ startXPos + i * (slotSize), pos.y }); // (1) slot even spacing
            childTc->setPos({ startXPos, pos.y });
            tc_.setPos({ startXPos, pos.y });
            rc_.render(HkSceneManagement::get().getSceneDataRef().sceneProjMatrix, tc_.getModelMatrix());

            startXPos += childTc->getScale().x;
            i++;
        }

        // (void)parentTc;
        // (void)childTcs;
    }

    // ConstraintParams..
};
} // hkui