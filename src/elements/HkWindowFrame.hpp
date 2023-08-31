#pragma once

#include <string>
#include <memory>

#include "../base/IHkRootNode.hpp"
#include "../base/HkNodeBase.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"
#include "HkContainer.hpp"
#include "HkButton.hpp"

namespace hkui
{
enum class HkWindowFrameMode
{
    Grabbable,
    FullScreenFixed
};

class HkWindowFrame : public HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    /* IHRootNode */
    void rootUpdateMySelf() override;

    /* HkNodeBase */
    void onScroll() override;
    void onDrag() override;
    void onWindowResize() override;
    void resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&,
        const HkScrollbarsSize) override;

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();

    /* Setters */
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);
    void setConstraintPolicy(const HkConstraintPolicy policy);
    void setWindowMode(const HkWindowFrameMode mode);

private:
    HkButton minimizeBtn_;
    HkButton exitBtn_;
    HkContainer wfCont_;

    HkWindowFrameMode mode_;
    glm::ivec2 cachedScale_;
    glm::ivec2 cachedPos_;
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

