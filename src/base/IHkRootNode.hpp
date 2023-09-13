#pragma once

#include <memory>
#include "HkNodeBase.hpp"

namespace hkui
{
class HkNodeBase;
class IHkRootNode
{
public:
    virtual void rootUpdate() = 0;
    virtual HkNodeBase* getUnderlayingNode() = 0;
    virtual ~IHkRootNode() = default;
};
using IHkRootNodeCPtr = const std::shared_ptr<IHkRootNode>;
using IHkRootNodePtr = std::shared_ptr<IHkRootNode>;
} // hkui
