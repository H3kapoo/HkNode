#pragma once

#include <string>
#include <memory>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{

class HkContainer
{
public:
    HkContainer(const std::string& name);

    //HkNode
    void updateMySelf();

    void updateChildren();
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren);

    /* Funcs to be removed when styling context comes */
    void printTree();
    void setColor(const glm::vec3& color);

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui