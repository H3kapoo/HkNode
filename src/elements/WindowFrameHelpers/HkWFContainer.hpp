#pragma once

#include "../../management/HkSceneManagement.hpp"
#include "../../base/HkNode.hpp"

namespace hkui
{
class HkWFContainer : public HkNode
{
public:
    HkWFContainer(const std::string& topContainerName);

    // HkNode
    void updateMySelf() override;

    void updateChildren();
    void pushChildren(const std::vector<HkNodePtr>& newChildren);

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkWFContainerPtr = std::shared_ptr<HkWFContainer>;
using HkWFContainerCPtr = const std::shared_ptr<HkWFContainer>;
} // hkui