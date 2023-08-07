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

    glm::mat4& getModelMatrix();

    void clearDiff();
    bool isPosInsideOfNode(const glm::ivec2& pin);

    /* Note: Even if these are public, still use the setters, they do more work under the hood! */
    glm::ivec2 scale, rot, pos;
private:
    void computeModelMatrix();

    glm::ivec2 lastPos, currLastDiff, diffCpy{ 0,0 };
    glm::mat4 modelMat;
};
} // hkui