#include "HkTransformContext.hpp"

namespace hkui
{
HkTransformContext::HkTransformContext()
    : pos{ 100,100 }, offsetFromParent{ 0,0 }, scale{ 200,100 }
{
    computeModelMatrix();
}

void HkTransformContext::addPos(const glm::vec2& pos)
{
    this->pos += pos;
    computeModelMatrix();
}

void HkTransformContext::setPos(const glm::vec2& pos)
{
    this->pos = pos;
    computeModelMatrix();
}

void HkTransformContext::addScale(const glm::vec2& scale)
{
    this->scale += scale;
    computeModelMatrix();
}

void HkTransformContext::setScale(const glm::vec2& scale)
{
    this->scale = scale;
    computeModelMatrix();
}

glm::vec2& HkTransformContext::getPos() { return pos; }

glm::mat4& HkTransformContext::getModelMatrix() { return modelMat; }

void HkTransformContext::setOffsetFromParent(const glm::vec2& offset)
{
    offsetFromParent = offset;
    // pos += offsetFromParent;
    computeModelMatrix();
}

void HkTransformContext::addOffsetFromParent(const glm::vec2& offset)
{
    offsetFromParent += offset;
    // pos += offsetFromParent;
    computeModelMatrix();
}

bool HkTransformContext::isPosInsideOfNode(const glm::vec2& pin)
{
    // for now we assume that nodes have the pivot point at the center of the shape
    // so the middle is height/2 middle/2
    // TODO: optimize
    glm::vec2 workingPos = pos + offsetFromParent;
    float xmin = workingPos.x - scale.x / 2;
    float xmax = workingPos.x + scale.x / 2;

    float ymin = workingPos.y - scale.y / 2;
    float ymax = workingPos.y + scale.y / 2;
    if (xmin < pin.x && pin.x < xmax && ymin < pin.y && pin.y < ymax)
        return true;
    return false;
}

//TODO: It would be better to pivot quad around top-left vertex instead of its center
void HkTransformContext::computeModelMatrix()
{
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(pos + offsetFromParent, -1.0f)); // it goes negative, expected
    // modelMat = glm::translate(modelMat, glm::vec3(pivotPoint_, 0));
    // modelMat = glm::rotate(modelMat, glm::radians(glm::degrees(rot_)), glm::vec3(0, 0, 1));
    modelMat = glm::scale(modelMat, glm::vec3(scale, 1.0f));
    // modelMat = glm::translate(modelMat, glm::vec3(-pivotPoint_, 0));
}

} // hkui