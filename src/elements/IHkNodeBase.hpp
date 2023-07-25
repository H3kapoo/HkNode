#pragma once

#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"

namespace hkui
{
class IHkNodeBase
{
    friend class HkWindowFrame;
    friend class HkWFContainer;
public:
    virtual ~IHkNodeBase() = default;
private:
    /* These functions will be accessible to friend classes but will not be accessible to user */
    virtual HkNodeData* accessNode() = 0;
    virtual HkTreeStructure<IHkNodeBase>* accessTreeStruct() = 0;
    virtual void updateMySelf() = 0;
};

using IHkNodeBasePtr = std::shared_ptr<IHkNodeBase>;
using IHkNodeBaseCPtr = const std::shared_ptr<IHkNodeBase>;
} // hkui