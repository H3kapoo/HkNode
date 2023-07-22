#pragma once

#include "../../management/HkSceneManagement.hpp"
#include "../../base/HkNode.hpp"

namespace hkui
{
class HkWFTopContainer : public HkNode
{
public:
    HkWFTopContainer(const std::string& topContainerName);
    // HkNode
    void updateMySelf() override;

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkWFTopContainerPtr = std::shared_ptr<HkWFTopContainer>;
using HkWFTopContainerCPtr = const std::shared_ptr<HkWFTopContainer>;
} // hkui