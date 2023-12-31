#pragma once

#include "../management/HkWindowData.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"

namespace hkui
{

enum class HkNodeType
{
    Invalid,
    RootWindowFrame,
    Container,
    ScrollBar,
    Knob,
    Button,
    ImageView,
    Label
};

struct HkNodeInfo
{
    std::string name;
    HkNodeType type;
    uint32_t id;
};

class HkNodeBase;
using HkTreeStruct = std::vector<HkTreeStructure<HkNodeBase, HkNodeType>*>;

class HkNodeBase
{
    friend class HkWindowManager;
    friend class HkSceneRenderer;

    /* Add here newly created UI Elements */
    friend class HkWindowFrame;
    friend class HkContainer;
    friend class HkButton;
    friend class HkScrollBar;
    friend class HkLabel;
    friend class HkKnob;

    friend class HkConstraintContext;
    friend class HkEventsContext;

    friend class HkPinchHelper;

public:
    HkNodeBase(const std::string& windowName, const HkNodeType& type);

    /* Getters */
    virtual HkStyleContext& getStyle();
    virtual HkEventsContext& getEvents();
    virtual const HkTransformContext& getTransform() const;
    virtual HkNodeInfo getNodeInfo();

    virtual ~HkNodeBase() = default;
private:
    /* These functions/params will be accessible to friend classes but will not be accessible to user */
    /* Derived can override whatever function it needs */
    virtual void renderMySelf();
    virtual void updateMySelf(const bool isSubWindowMinimized);

    /* Constraints */
    virtual void resolveChildrenConstraints(HkTreeStruct& children,
        const HkScrollbarsSize& sbSizes = {});

    /* Events */
    virtual void onDrag();
    virtual void onAnimationFrameRequested();
    virtual void onClick();
    virtual void onRelease();
    virtual void onScroll();
    virtual void onGeneralUpdate();
    virtual void onWindowResize();
    virtual void onGeneralMouseMove();
    virtual void onGeneralMouseClickOrRelease();
    virtual void onGeneralMouseScroll();
    virtual void onFirstHeartbeat();
    virtual void onResolveFocusPass();
    virtual void onKeyAction();
    virtual void onCharAction();
    virtual void onDirtyAttribs(const std::unordered_set<HkStyleDirtyAttribs>&);

    virtual void postRenderAdditionalDetails();

    /* Normal internal functions */
    void resolveHover();
    void resolveNearestActiveScrollbar();
    void resolveFocus();
    void resolveMouseClickEvent();
    void resolveMouseScrollEvent();
    void resolveMouseMovementEvent();
    void resolveDirtyAttributes();

    void setSelectTransparent(const bool isTransparent);

    /* Injects */
    virtual void injectWindowDataPtr(HkWindowData* windowDataPtr);

private:
    bool hadFirstHeartBeat_{ false };
    bool isTransparent_{ false };
    HkNodeData node_;
    HkTreeStructure<HkNodeBase, HkNodeType> treeStruct_;
    HkWindowData* windowDataPtr_;
};

using HkNodeBasePtr = std::shared_ptr<HkNodeBase>;
using HkNodeBaseCPtr = const std::shared_ptr<HkNodeBase>;
} // hkui