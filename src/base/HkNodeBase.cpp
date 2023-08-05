#include "HkNodeBase.hpp"

namespace hkui
{
HkNodeBase::HkNodeBase(const std::string& windowName, const std::string& type)
    : treeStruct_(this, windowName, type)
    , sceneDataRef_(HkSceneManagement::get().getSceneDataRef())
{
    node_.constraintContext.setRootTc(&node_.transformContext);
}

void HkNodeBase::updateMySelf()
{
    /*main HkEvents handler*/
    switch (sceneDataRef_.currentEvent)
    {
    case HkEvent::None: break;
    case HkEvent::HoverScan: resolveHover(); break;
    case HkEvent::FocusScan: resolveFocus(); break;
    case HkEvent::GeneralUpdate: onGeneralUpdate(); break;
    case HkEvent::WindowResize: onWindowResize(); break;
    case HkEvent::MouseMove: onGeneralMouseMove(); break;
    case HkEvent::MouseClick: onGeneralMouseClick(); break;
    case HkEvent::MouseEnterExit: break;
    case HkEvent::MouseScroll: break;
    case HkEvent::DropPath: break;
    }

    //TODO: Definitely smart optimize this out..we dont want to create a vec each frame just for this
    auto& children = treeStruct_.getChildren();
    resolveConstraints(children);

    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
    for (const auto& child : children)
        child->getPayload()->updateMySelf();
}

void HkNodeBase::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    node_.constraintContext.resolveConstraints(children);
}

void HkNodeBase::resolveHover()
{
    if (node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
    {
        sceneDataRef_.hoveredId = treeStruct_.getId();
    }
}

void HkNodeBase::resolveFocus()
{
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.clickedMouseButton == HkMouseButton::Left
        && node_.transformContext.isPosInsideOfNode(sceneDataRef_.mousePos))
    {
        sceneDataRef_.focusedId = treeStruct_.getId();
        sceneDataRef_.mouseOffsetFromFocusedCenter = node_.transformContext.pos - sceneDataRef_.mousePos;
    }
}

void HkNodeBase::onGeneralUpdate() {}
void HkNodeBase::onWindowResize() {}
void HkNodeBase::onGeneralMouseMove() {}
void HkNodeBase::onGeneralMouseClick() {}
} // hkui