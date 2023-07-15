#pragma once

#include <string>
#include <memory>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{
class HkButton : public HkNode
{
public:
    HkButton(const std::string& name);

    //HkNode
    void updateMySelf() override;
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;

} // hkui