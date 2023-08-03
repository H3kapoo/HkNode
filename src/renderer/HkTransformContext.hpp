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

    void addPos(const glm::ivec2& pos);
    void setPos(const glm::ivec2& pos);

    void addScale(const glm::ivec2& scale);
    void setScale(const glm::ivec2& scale);

    glm::ivec2& getPos();
    glm::ivec2& getScale();
    glm::mat4& getModelMatrix();

    void clearDiff();
    bool isPosInsideOfNode(const glm::ivec2& pin);

private:
    void computeModelMatrix();

    glm::ivec2 scale, rot;
    glm::ivec2 currentPos, lastPos, currLastDiff, diffCpy{ 0,0 };
    glm::mat4 modelMat;
};
} // hkui