#pragma once

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"

namespace hkui
{
class HkNodeBase
{
    friend class HkSceneManagement;
    friend class HkWindowFrame;
    friend class HkContainer;

    friend class HkScrollBar;
    friend class HkKnob;

    friend class HkConstraintContext;

public:
    HkNodeBase(const std::string& windowName, const std::string& type);
    virtual ~HkNodeBase() = default;
private:
    /* These functions/params will be accessible to friend classes but will not be accessible to user */
    /* Derived can override whatever function it needs */
    virtual void updateMySelf();

    /* Constraints */
    virtual void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);

    /* Events */
    virtual void onDrag();
    virtual void onClick();

    virtual void onGeneralUpdate();
    virtual void onWindowResize();
    virtual void onGeneralMouseMove();
    virtual void onGeneralMouseClick();

    virtual void postRenderAdditionalDetails();

    /* Normal internal functions */
    void resolveHover();
    void resolveFocus();
    void resolveMouseClickEvent();
    void resolveMouseMovementEvent();


    HkNodeData node_;
    HkTreeStructure<HkNodeBase> treeStruct_;
    HkSceneData& sceneDataRef_; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkNodeBasePtr = std::shared_ptr<HkNodeBase>;
using HkNodeBaseCPtr = const std::shared_ptr<HkNodeBase>;
} // hkui