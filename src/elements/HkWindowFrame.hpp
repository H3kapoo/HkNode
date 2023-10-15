#pragma once

#include <string>
#include <memory>

#include "../base/IHkRootNode.hpp"
#include "../base/HkNodeBase.hpp"
#include "../base/HkTreeStructure.hpp"
#include "../utils/HkPinchHelper.hpp"
#include "HkContainer.hpp"
#include "HkButton.hpp"

namespace hkui
{
enum class HkWindowFrameMode
{
    Grabbable,
    FullScreenFixed
};

class HkWindowFrame : public HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();

    /* Setters */
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);
    void setWindowMode(const HkWindowFrameMode mode);

    /* Getters */
    bool isAlive() const override;
    HkStyleContext& getStyle() override;
    const HkTransformContext& getTransform() const override;

private:
    /* IHRootNode */
    void rootUpdate() override;
    HkNodeBase* getUnderlayingNode() override;

    /* HkNodeBase */
    void onFirstHeartbeat() override;
    void postRenderAdditionalDetails() override;
    void onAnimationFrameRequested() override;
    void onScroll() override;
    void onDrag() override;
    void onClick() override;
    void onRelease() override;
    void onGeneralMouseMove() override;
    void onWindowResize() override;
    void resolveChildrenConstraints(HkTreeStruct&,
        const HkScrollbarsSize&) override;

    void cursorChange(const int32_t value);

    /* Injects */
    void injectWindowDataPtr(HkWindowData* windowDataPtr) override;

    HkButton minimizeBtn_;
    HkButton exitBtn_;
    HkContainer wfCont_;


    bool stillAlive_{ true };
    HkWindowFrameMode mode_;
    glm::ivec2 cachedScale_;
    glm::ivec2 cachedPos_;
    bool isMinimized_{ false };
    bool isAnimOngoing{ false };
    bool restarted{ false };
    float t{ 0.0f };
    glm::vec2 startPos, endPos;
    bool dummyVal{ false };

    double startTime{ 0.0f };
    double animDuration{ 0.25f };

    HkPinchHelper pinchHelper_;
    glm::ivec2 boundPos_, boundScale_;

};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

