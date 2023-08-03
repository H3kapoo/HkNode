#pragma once

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeBase.hpp"

namespace hkui
{
class HkContainer : public HkNodeBase
{
public:
    HkContainer(const std::string& containerName);

    void onGeneralUpdate() override; // HkNodeBase
    void onGeneralMouseMove() override; // HkNodeBase

    void repositionBasedOnParent();

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
};
} // hkui