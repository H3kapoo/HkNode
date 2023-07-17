#pragma once

#include <memory>

namespace hkui
{
class IHkRootNode
{
public:
    virtual void rootUpdateMySelf() = 0;
    virtual ~IHkRootNode() = default;
};
using IHkRootNodeCPtr = const std::shared_ptr<IHkRootNode>;
using IHkRootNodePtr = std::shared_ptr<IHkRootNode>;
} // hkui
