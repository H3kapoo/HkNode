#include "HkNodeBase.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const std::string& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef()) {}

void HkNodeBase::updateMySelf()
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

    //TODO: Constraintor should be here, something like
    // node_.constraintContext()

    // render parent -> update element children
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    // std::cout << glfwGetTime() << " Rendered " << treeStruct_.getName() << '\n';
    for (const auto& child : treeStruct_.getChildren())
        child->getPayload()->updateMySelf();
}

void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
} // hkui