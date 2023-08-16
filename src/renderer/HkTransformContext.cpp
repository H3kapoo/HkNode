#include "HkTransformContext.hpp"

#include <iostream>


namespace hkui
{
HkTransformContext::HkTransformContext()
    : scale{ 200,100 }, rot{ 0,0 }, pos{ 100,100 }
{
    computeModelMatrix();
}

void HkTransformContext::addPos(const glm::ivec2& pos)
{
    this->pos += pos;
    computeModelMatrix();
}

void HkTransformContext::setPos(const glm::ivec2& pos)
{
    this->pos = pos;
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

bool HkTransformContext::isPosInsideOfNode(const glm::ivec2& posIn) const
{
    if (pos.x <= posIn.x && posIn.x <= pos.x + scale.x && pos.y <= posIn.y && posIn.y <= pos.y + scale.y)
        return true;
    return false;
}

HKAxisBoundsPoints HkTransformContext::computeXIntersectionPointWith(const HkTransformContext& otherTc) const
{
    HKAxisBoundsPoints xBounds = {};

    /* Case when otherTc X is fully overflowing Tc */
    if (otherTc.pos.x < pos.x && (otherTc.pos.x + otherTc.scale.x) >(pos.x + scale.x))
    {
        xBounds.firstPos = pos.x;
        xBounds.secondPos = pos.x + scale.x;
        return xBounds;
    }

    /* Case when otherTc X is fully inside Tc */
    if (otherTc.pos.x >= pos.x && (otherTc.pos.x + otherTc.scale.x) <= (pos.x + scale.x))
    {
        xBounds.firstPos = otherTc.pos.x;
        xBounds.secondPos = otherTc.pos.x + otherTc.scale.x;
        return xBounds;
    }

    /* Case when otherTc X partially lies outside of Tc to the RIGHT */
    if (otherTc.pos.x < (pos.x + scale.x) && (otherTc.pos.x + otherTc.scale.x) > pos.x + scale.x)
    {
        xBounds.firstPos = otherTc.pos.x;
        xBounds.secondPos = otherTc.pos.x + (pos.x + scale.x) - otherTc.pos.x;
        return xBounds;
    }

    /* Case when otherTc X partially lies outside of Tc to the LEFT */
    if (otherTc.pos.x < pos.x && (otherTc.pos.x + otherTc.scale.x) > pos.x)
    {
        xBounds.firstPos = pos.x;
        xBounds.secondPos = pos.x + otherTc.scale.x - (pos.x - otherTc.pos.x);
        return xBounds;
    }

    return xBounds;
}

HkTransformBBox HkTransformContext::computeBBoxWith(const HkTransformContext& otherTc) const
{
    HkTransformBBox returnTc = {};
    returnTc.pos.y = 0;
    returnTc.scale.y = 0;
    returnTc.pos.x = computeXIntersectionPointWith(otherTc).firstPos;
    returnTc.scale.x = computeXIntersectionPointWith(otherTc).secondPos - computeXIntersectionPointWith(otherTc).firstPos;

    return returnTc;
}

//TODO: It would be better to pivot quad around top-left vertex instead of its center
void HkTransformContext::computeModelMatrix()
{
    /*
        NDC Screen looks like:
                           1
        =========================================
        |                  .                    |
        |                  .                    |
        |                  . (0,0)              |
     -1 | -----------------.------------------- |  1
        |                  .                    |
        |                  .                    |
        |                  .                    |
        =========================================
                         -1

        Our quad mesh looks like:
        (-0.5,-0.5) --------  (0.5,-0.5)
                   |        |
                   | (0,0)  |
                   |        |
        (-0.5,0.5) ---------- (0.5,0.5)

        Adding 0.5 to XY puts 0,0 at top left corner :
        (0.0,0.0) ---------------- (1.0, 0.0)
                  |              |
                  |  (0.5,0.5)   |
                  |              |
        (0.0,1.0) ---------------- (1.0,1.0)

        Scaling occurs: ...
        Move back to old center by subtracting 0.5: ...
        Position element normally where it should go: ...
        NOTE: Rotation and scaling occurs from 0,0 or else we will get distorted objects
              General idea is to subtract/add (depends) the pivot position from each vertex of the mesh
              => do scaling => do rotate => add/subtract back the pivot => move object wherever normally
    */
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(pos, -1.0f)); // it goes negative, expected
    modelMat = glm::translate(modelMat, glm::vec3(-0.5f, -0.5f, 0));
    modelMat = glm::scale(modelMat, glm::vec3(scale, 1.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.5f, 0.5f, 0));
}
} // hkui