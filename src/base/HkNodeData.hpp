#pragma once

#include "../renderer/HkRenderContext.hpp"
#include "../renderer/HkTransformContext.hpp"

namespace hkui
{
/*This class exists so that to not pollute HkNode with unnecessary information that doesn't really belong there.
  We dont want these context variables to be available to the user, but we want them to be available inside the
  UI node classes so that we can, for example, access render/transform data about parents/children. The only
  way I can see to do this is by using friend classes.. maybe there will be a better option in the future.
*/
class HkNodeData
{
public:
    /*UI elements need to be friend so that they can access contexts bellow*/
    friend class HkWindowFrame;
    friend class HkContainer;
    friend class HkButton;
    // friend class HkButton;

    virtual ~HkNodeData() = default;
private:
    HkRenderContext renderContext;
    HkTransformContext transformContext;
};
} // hkui
