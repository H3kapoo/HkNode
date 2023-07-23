#include "HkTransformContext.hpp"

#include <iostream>

namespace hkui
{
HkTransformContext::HkTransformContext()
    : scale{ 200,100 }, currentPos{ 100,100 }, lastPos{ 100,100 }, currLastDiff{ 0,0 }
{
    computeModelMatrix();
}

void HkTransformContext::addPos(const glm::vec2& pos)
{
    lastPos = currentPos;
    currentPos += pos;
    currLastDiff = currentPos - lastPos;
    // std::cout << "Diff: " << currLastDiff.x << " " << currLastDiff.y << '\n';

    computeModelMatrix();
}

void HkTransformContext::setPos(const glm::vec2& pos)
{
    lastPos = currentPos;
    currentPos = pos;
    currLastDiff = currentPos - lastPos;
    // std::cout << "Diff: " << currLastDiff.x << " " << currLastDiff.y << '\n';

    computeModelMatrix();
}

/* Use this for initialization of transform context */
void HkTransformContext::init(const glm::vec2& pos)
{
    lastPos = currentPos = pos;
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

glm::vec2& HkTransformContext::getPos() { return currentPos; }

glm::vec2& HkTransformContext::getScale() { return scale; }

glm::vec2& HkTransformContext::getDiff() { return currLastDiff; }

glm::mat4& HkTransformContext::getModelMatrix() { return modelMat; }

/* This shall be called at the end of update loop of each node holding children */
void HkTransformContext::clearDiff() { currLastDiff = { 0,0 }; }

/*TO BE DEPRECATED, use addPos*/
void HkTransformContext::setOffsetFromParent(const glm::vec2& offset)
{
    currentPos += offset;
    computeModelMatrix();
}

bool HkTransformContext::isPosInsideOfNode(const glm::vec2& pin)
{
    // for now we assume that nodes have the pivot point at the center of the shape
    // so the middle is height/2 middle/2
    // TODO: optimize
    // glm::vec2 workingPos = pos + offsetFromParent;
    float xmin = currentPos.x - scale.x / 2;
    float xmax = currentPos.x + scale.x / 2;

    float ymin = currentPos.y - scale.y / 2;
    float ymax = currentPos.y + scale.y / 2;
    if (xmin < pin.x && pin.x < xmax && ymin < pin.y && pin.y < ymax)
        return true;
    return false;
}

//TODO: It would be better to pivot quad around top-left vertex instead of its center
void HkTransformContext::computeModelMatrix()
{
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(currentPos, -1.0f)); // it goes negative, expected
    // modelMat = glm::translate(modelMat, glm::vec3(pivotPoint_, 0));
    // modelMat = glm::rotate(modelMat, glm::radians(glm::degrees(rot_)), glm::vec3(0, 0, 1));
    modelMat = glm::scale(modelMat, glm::vec3(scale, 1.0f));
    // modelMat = glm::translate(modelMat, glm::vec3(-pivotPoint_, 0));
}

} // hkui