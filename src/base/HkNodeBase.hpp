#pragma once

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"

namespace hkui
{
class HkNodeBase
{
    friend class HkWindowFrame;
    friend class HkWFContainer;
public:
    HkNodeBase(const std::string& windowName, const std::string& type)
        : treeStruct_(this, windowName, type)
        , sceneDataRef_(HkSceneManagement::get().getSceneDataRef()) {}
    virtual ~HkNodeBase() = default;
private:
    /* These functions/params will be accessible to friend classes but will not be accessible to user */
    virtual void updateMySelf()
    {
        /*main HkEvents handler*/
        switch (sceneDataRef_.currentEvent)
        {
        case HkEvent::None: break;
        case HkEvent::HoverScan:
            if (node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
            {
                sceneDataRef_.hoveredId = treeStruct_.getId();
            }
            break;
        case HkEvent::FocusScan:
            // Focus part
            if (sceneDataRef_.isMouseClicked && sceneDataRef_.clickedMouseButton == HkMouseButton::Left
                && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
            {
                sceneDataRef_.focusedId = treeStruct_.getId();
                sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.getPos() - sceneDataRef_.mousePos;
            }
            break;
        case HkEvent::GeneralUpdate: onGeneralUpdate(); break;
        case HkEvent::WindowResize: onWindowResize(); break;
        case HkEvent::MouseMove: onGeneralMouseMove(); break;
        case HkEvent::MouseClick: onGeneralMouseClick(); break;
        case HkEvent::MouseEnterExit: break;
        case HkEvent::MouseScroll: break;
        case HkEvent::DropPath: break;
        }

        // render parent -> update element children
        node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
        for (const auto& child : treeStruct_.getChildren())
            child->getPayload()->updateMySelf();
    }

    virtual void onGeneralUpdate() {}
    virtual void onWindowResize() {}
    virtual void onGeneralMouseMove() {}
    virtual void onGeneralMouseClick() {}

    HkNodeData node_;
    HkTreeStructure<HkNodeBase> treeStruct_;
    HkSceneData& sceneDataRef_; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkNodeBasePtr = std::shared_ptr<HkNodeBase>;
using HkNodeBaseCPtr = const std::shared_ptr<HkNodeBase>;
} // hkui