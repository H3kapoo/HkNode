#pragma once

#include <string>
#include <memory>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{

class HkContainer : public HkNode
{
public:
    HkContainer(const std::string& name);
    //HkNode
    void updateMySelf() override;

    void updateChildren();
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren);

    /* Funcs to be removed when styling context comes */
    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkContainerUPtr = std::unique_ptr<HkContainer>;
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui