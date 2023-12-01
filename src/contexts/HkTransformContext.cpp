#include "HkTransformContext.hpp"

#include <iostream>

namespace hkui
{
HkTransformContext::HkTransformContext()
    : scale{ 200, 100 }, rot{ 0,0 }, pos{ 100,100 }
    , cScale{ 200, 100 }, cRot{ 0,0 }, cPos{ 100,100 }
    , vScale{ 200, 100 }, vPos{ 100, 100 }
{}

/* Absolute positioning. Doesn't take into account if position can actually be seen by user */
bool HkTransformContext::isPosInsideOfNode(const glm::ivec2& posIn) const
{
    if (pos.x <= posIn.x && posIn.x <= pos.x + scale.x && pos.y <= posIn.y && posIn.y <= pos.y + scale.y)
        return true;
    return false;
}

/* Non-absolute positioning. Takes into account if position can actually be seen by user */
bool HkTransformContext::isPosInsideOfNodeViewableArea(const glm::ivec2& posIn) const
{
    if (vPos.x <= posIn.x && posIn.x <= vPos.x + vScale.x && vPos.y <= posIn.y && posIn.y <= vPos.y + vScale.y)
        return true;
    return false;
}

bool HkTransformContext::isAnyDifference() const
{
    if (cPos.x == prevPos.x && cPos.y == prevPos.y && cScale.x == prevScale.x && cScale.y == prevScale.y)
        // if (pos.x == prevPos.x && pos.y == prevPos.y && scale.x == prevScale.x && scale.y == prevScale.y)
        return false;
    return true;
}

void HkTransformContext::copyContentDataToAbsoluteData()
{
    pos = cPos;
    scale = cScale;
    rot = cRot;
}

/* Simply computes the axis aligned Bounding Box between this TC and otherTC */
HkTransformBBox HkTransformContext::computeBBoxWith(const HkTransformContext& otherTc) const
{
    HkTransformBBox returnTc = {};
    const auto xBounds = computeXIntersectionPointsWith(otherTc);
    const auto yBounds = computeYIntersectionPointsWith(otherTc);
    returnTc.pos.x = xBounds.firstPos;
    returnTc.scale.x = xBounds.secondPos - xBounds.firstPos;
    returnTc.pos.y = yBounds.firstPos;
    returnTc.scale.y = yBounds.secondPos - yBounds.firstPos;

    return returnTc;
}

/* Compute the intersection points between TC and otherTC along X axis */
HKAxisBoundsPoints HkTransformContext::computeXIntersectionPointsWith(const HkTransformContext& otherTc) const
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

/* Compute the intersection points between TC and otherTC along X axis */
HKAxisBoundsPoints HkTransformContext::computeYIntersectionPointsWith(const HkTransformContext& otherTc) const
{
    HKAxisBoundsPoints yBounds = {};

    /* Case when otherTc Y is fully overflowing Tc */
    if (otherTc.pos.y < pos.y && (otherTc.pos.y + otherTc.scale.y) >(pos.y + scale.y))
    {
        yBounds.firstPos = pos.y;
        yBounds.secondPos = pos.y + scale.y;
        return yBounds;
    }

    /* Case when otherTc Y is fully inside Tc */
    if (otherTc.pos.y >= pos.y && (otherTc.pos.y + otherTc.scale.y) <= (pos.y + scale.y))
    {
        yBounds.firstPos = otherTc.pos.y;
        yBounds.secondPos = otherTc.pos.y + otherTc.scale.y;
        return yBounds;
    }

    /* Case when otherTc Y partially lies outside of Tc to the TOP */
    if (otherTc.pos.y < (pos.y + scale.y) && (otherTc.pos.y + otherTc.scale.y) > pos.y + scale.y)
    {
        yBounds.firstPos = otherTc.pos.y;
        yBounds.secondPos = otherTc.pos.y + (pos.y + scale.y) - otherTc.pos.y;
        return yBounds;
    }

    /* Case when otherTc Y partially lies outside of Tc to the BOTTOM */
    if (otherTc.pos.y < pos.y && (otherTc.pos.y + otherTc.scale.y) > pos.y)
    {
        yBounds.firstPos = pos.y;
        yBounds.secondPos = pos.y + otherTc.scale.y - (pos.y - otherTc.pos.y);
        return yBounds;
    }

    return yBounds;
}

/* Compute the model matrix of this TC. Used for rendering */
void HkTransformContext::computeModelMatrix()
{
    /* Massive optimization in order to not calculate the same model matrix if nothing chaged */
    if (!isAnyDifference())
    {
        return;
    }

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
    // modelMat = glm::mat4(1.0f);
    // modelMat = glm::translate(modelMat, glm::vec3(pos, -1.0f)); // it goes negative, expected
    // modelMat = glm::translate(modelMat, glm::vec3(-0.5f, -0.5f, 0));
    // modelMat = glm::scale(modelMat, glm::vec3(scale, 1.0f));
    // modelMat = glm::translate(modelMat, glm::vec3(0.5f, 0.5f, 0));

    // /* Fix for off-by-one pixel rendering error. This is caused because we do calculations with respect to
    //    top left corner of 1x1 square*/
    // modelMat[3][0] = ceil(modelMat[3][0]);
    // modelMat[3][1] = ceil(modelMat[3][1]);
    // if ((int)modelMat[0][0] % 2 == 1) { modelMat[3][0] += 1; }
    // if ((int)modelMat[1][1] % 2 == 1) { modelMat[3][1] += 1; }

    // prevPos = pos;
    // prevScale = scale;
    modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, glm::vec3(cPos, -1.0f)); // it goes negative, expected
    modelMat = glm::translate(modelMat, glm::vec3(-0.5f, -0.5f, 0));
    modelMat = glm::scale(modelMat, glm::vec3(cScale, 1.0f));
    modelMat = glm::translate(modelMat, glm::vec3(0.5f, 0.5f, 0));

    /* Fix for off-by-one pixel rendering error. This is caused because we do calculations with respect to
       top left corner of 1x1 square*/
    modelMat[3][0] = ceil(modelMat[3][0]);
    modelMat[3][1] = ceil(modelMat[3][1]);
    if ((int)modelMat[0][0] % 2 == 1) { modelMat[3][0] += 1; }
    if ((int)modelMat[1][1] % 2 == 1) { modelMat[3][1] += 1; }

    prevPos = cPos;
    prevScale = cScale;
}

void HkTransformContext::addPos(const glm::ivec2& pos)
{
    this->pos += pos;
}

void HkTransformContext::setPos(const glm::ivec2& pos)
{
    this->pos = pos;
}

void HkTransformContext::addContentPos(const glm::ivec2& cPos)
{
    this->cPos += cPos;
}

void HkTransformContext::setContentPos(const glm::ivec2& cPos)
{
    this->cPos = cPos;
}

void HkTransformContext::addScale(const glm::ivec2& scale)
{
    this->scale += scale;
}

void HkTransformContext::setScale(const glm::ivec2& scale)
{
    this->scale = scale;
}

void HkTransformContext::addContentScale(const glm::ivec2& cScale)
{
    this->cScale += cScale;
}

void HkTransformContext::setContentScale(const glm::ivec2& cScale)
{
    this->cScale = cScale;
}

void HkTransformContext::setVPos(const glm::ivec2& vPos)
{
    this->vPos = vPos;
}

void HkTransformContext::setVScale(const glm::ivec2& vScale)
{
    this->vScale = vScale;
}

const glm::ivec2& HkTransformContext::getVPos() const
{
    return vPos;
}

const glm::ivec2& HkTransformContext::getVScale() const
{
    return vScale;
}

const glm::ivec2& HkTransformContext::getPos() const
{
    return pos;
}

const glm::ivec2& HkTransformContext::getScale() const
{
    return scale;
}

const glm::ivec2& HkTransformContext::getContentPos() const
{
    return cPos;
}

const glm::ivec2& HkTransformContext::getContentScale() const
{
    return cScale;
}

const glm::mat4& HkTransformContext::getModelMatrix()
{
    computeModelMatrix();
    return modelMat;
}
} // hkui