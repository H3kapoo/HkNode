#pragma once

#include <string>
#include <memory>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{
/* This shall serve as the root of the scene manager, it has no parent and can only host children.*/
// TODO: Make sure you cannot pushChildren of type HkWindowFrame
class HkWindowFrame : public HkNode
{
public:
    HkWindowFrame(const std::string windowName);

    // HkNode
    void updateMySelf() override;

    void updateChildren();
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren);
    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;

} // hkui

