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
public:
    HkNodeBase(const std::string& windowName, const std::string& type);
    virtual ~HkNodeBase() = default;
private:
    /* These functions/params will be accessible to friend classes but will not be accessible to user */
    virtual void updateMySelf();

    virtual void onGeneralUpdate();
    virtual void onWindowResize();
    virtual void onGeneralMouseMove();
    virtual void onGeneralMouseClick();

    HkNodeData node_;
    HkTreeStructure<HkNodeBase> treeStruct_;
    HkSceneData& sceneDataRef_; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkNodeBasePtr = std::shared_ptr<HkNodeBase>;
using HkNodeBaseCPtr = const std::shared_ptr<HkNodeBase>;
} // hkui