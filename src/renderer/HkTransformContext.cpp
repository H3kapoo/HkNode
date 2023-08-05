#include "HkTransformContext.hpp"

#include <iostream>

namespace hkui
{
HkTransformContext::HkTransformContext()
    : scale{ 200,100 }, pos{ 100,100 }, lastPos{ 100,100 }, currLastDiff{ 0,0 }
{
    computeModelMatrix();
}

void HkTransformContext::addPos(const glm::ivec2& pos)
{
    lastPos = pos;
    this->pos += pos;
    currLastDiff = pos - lastPos;
    computeModelMatrix();
}

void HkTransformContext::setPos(const glm::ivec2& pos)
{
    lastPos = pos;
    this->pos = pos;
    currLastDiff = pos - lastPos;
    computeModelMatrix();
}

void HkTransformContext::addScale(const glm::ivec2& scale)
{
    this->scale += scale;
    computeModelMatrix();
}

void HkTransformContext::setScale(const glm::ivec2& scale)
{
    this->scale = scale;
    computeModelMatrix();
}

glm::mat4& HkTransformContext::getModelMatrix() { return modelMat; }

bool HkTransformContext::isPosInsideOfNode(const glm::ivec2& pin)
{
    // for now we assume that nodes have the pivot point at the center of the shape
    // so the middle is height/2 middle/2
    // TODO: optimize
    // glm::ivec2 workingPos = pos + offsetFromParent;
    float xmin = pos.x - scale.x / 2;
    float xmax = pos.x + scale.x / 2;

    float ymin = pos.y - scale.y / 2;
    float ymax = pos.y + scale.y / 2;
    if (xmin < pin.x && pin.x < xmax && ymin < pin.y && pin.y < ymax)
        return true;
    return false;
}

//TODO: It would be better to pivot quad around top-left vertex instead of its center
void HkTransformContext::computeModelMatrix()
{
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(pos, -1.0f)); // it goes negative, expected
    // modelMat = glm::translate(modelMat, glm::vec3(pivotPoint_, 0));
    // modelMat = glm::rotate(modelMat, glm::radians(glm::degrees(rot_)), glm::vec3(0, 0, 1));
    modelMat = glm::scale(modelMat, glm::vec3(scale, 1.0f));
    // modelMat = glm::translate(modelMat, glm::vec3(-pivotPoint_, 0));
}

} // hkui