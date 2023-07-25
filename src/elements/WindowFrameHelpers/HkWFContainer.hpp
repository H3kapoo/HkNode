#pragma once

#include "../../management/HkSceneManagement.hpp"
#include "../../base/HkNode.hpp"

namespace hkui
{
class HkWFContainer
{
public:
    HkWFContainer(const std::string& topContainerName);

    // HkNode
    void updateMySelf();

    void updateChildren();
    void pushChildren(const std::vector<HkNodePtr>& newChildren);

private:
    HkNodePtr node;
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkWFContainerPtr = std::shared_ptr<HkWFContainer>;
using HkWFContainerCPtr = const std::shared_ptr<HkWFContainer>;
} // hkui