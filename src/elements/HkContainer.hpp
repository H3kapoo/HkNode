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

    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui