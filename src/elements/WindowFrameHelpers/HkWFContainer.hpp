#pragma once

#include "../../management/HkSceneManagement.hpp"
#include "../../base/HkNodeBase.hpp"

namespace hkui
{
class HkWFContainer : public HkNodeBase
{
public:
    HkWFContainer(const std::string& containerName);

    void onGeneralUpdate() override; // HkNodeBase
    void onGeneralMouseMove() override; // HkNodeBase

    void repositionBasedOnParent();

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
};
} // hkui