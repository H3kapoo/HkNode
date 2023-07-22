#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hkui
{
/* This context should basically handle all that happens to the model matrix of the primitive */
class HkTransformContext
{
public:
    HkTransformContext();

    void addPos(const glm::vec2& pos);
    void setPos(const glm::vec2& pos);
    void init(const glm::vec2& pos);

    void addScale(const glm::vec2& scale);
    void setScale(const glm::vec2& scale);

    glm::vec2& getPos();
    glm::vec2& getScale();
    glm::vec2& getDiff();
    glm::mat4& getModelMatrix();

    void clearDiff();
    void setOffsetFromParent(const glm::vec2& offset);
    bool isPosInsideOfNode(const glm::vec2& pin);

private:
    void computeModelMatrix();

    glm::vec2 scale, rot;
    glm::vec2 currentPos, lastPos, currLastDiff, diffCpy{ 0,0 };
    glm::mat4 modelMat;

    bool firstSetting{ true };
};
} // hkui