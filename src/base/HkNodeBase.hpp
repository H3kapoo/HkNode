#pragma once

#include "../management/HkSceneData.hpp"
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
    ImageView
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
    friend class HkSceneManagement;
    friend class HkSceneRenderer;

    friend class HkWindowFrame;
    friend class HkContainer;
    friend class HkButton;
    friend class HkScrollBar;
    friend class HkImageView;
    friend class HkKnob;

    friend class HkConstraintContext;
    friend class HkEventsContext;

public:
    HkNodeBase(const std::string& windowName, const HkNodeType& type);

    /* Getters */
    virtual HkStyleContext& getStyle();
    virtual HkEventsContext& getEvents();
    virtual HkNodeInfo getNodeInfo();

    virtual ~HkNodeBase() = default;
private:
    /* These functions/params will be accessible to friend classes but will not be accessible to user */
    /* Derived can override whatever function it needs */
    virtual void renderMySelf();
    virtual void updateMySelf();

    /* Constraints */
    virtual void resolveChildrenConstraints(HkTreeStruct& children,
        const HkScrollbarsSize& sbSizes = {});

    /* Events */
    virtual void onDrag();
    virtual void onClick();
    virtual void onRelease();
    virtual void onScroll();
    virtual void onGeneralUpdate();
    virtual void onWindowResize();
    virtual void onGeneralMouseMove();
    virtual void onGeneralMouseClick();
    virtual void onGeneralMouseScroll();

    virtual void postRenderAdditionalDetails();

    /* Normal internal functions */
    void resolveHover();
    void resolveNearestActiveScrollbar();
    void resolveFocus();
    void resolveMouseClickEvent();
    void resolveMouseScrollEvent();
    void resolveMouseMovementEvent();

private:
    HkNodeData node_;
    HkTreeStructure<HkNodeBase, HkNodeType> treeStruct_;
    HkSceneData& sceneDataRef_; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkNodeBasePtr = std::shared_ptr<HkNodeBase>;
using HkNodeBaseCPtr = const std::shared_ptr<HkNodeBase>;
} // hkui