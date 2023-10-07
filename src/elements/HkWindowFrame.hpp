#pragma once

#include <string>
#include <memory>

#include "../base/IHkRootNode.hpp"
#include "../base/HkNodeBase.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"
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

    /* IHRootNode */
    void rootUpdate() override;
    HkNodeBase* getUnderlayingNode() override;


    /* HkNodeBase */
    void onAnimationFrameRequested() override;
    void onScroll() override;
    void onDrag() override;
    void onWindowResize() override;
    void resolveChildrenConstraints(HkTreeStruct&,
        const HkScrollbarsSize&) override;

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();

    HkStyleContext& getStyle() override;

    /* Setters */
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);
    void setWindowMode(const HkWindowFrameMode mode);

    /* Getters */
    bool isAlive() const override;

    /* Injects */
    void injectWindowDataPtr(HkWindowData* windowDataPtr) override;

private:
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
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

