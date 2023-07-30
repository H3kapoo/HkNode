#pragma once

#include "../../management/HkSceneManagement.hpp"
#include "../../base/HkNodeBase.hpp"

namespace hkui
{
class HkWFContainer : public HkNodeBase
{
public:
    HkWFContainer(const std::string& containerName);

    void updateMySelf() override; // HkNodeBase

    void repositionBasedOnParent();

    void updateChildren();
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
};
} // hkui