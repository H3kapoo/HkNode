#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hkui
{

struct HkTransformBBox
{
    glm::ivec2 pos, scale;
};

struct HKAxisBoundsPoints
{
    int firstPos, secondPos;
};

/* This context should basically handle all that happens to the model matrix of the primitive */
class HkTransformContext
{
public:
    HkTransformContext();

    void addPos(const glm::ivec2& pos);
    void setPos(const glm::ivec2& pos);

    void addScale(const glm::ivec2& scale);
    void setScale(const glm::ivec2& scale);

    void setVScale(const glm::ivec2& vScale);
    void setVPos(const glm::ivec2& vPos);

    const glm::ivec2& getVPos() const;
    const glm::ivec2& getVScale() const;

    glm::mat4& getModelMatrix();

    bool isPosInsideOfNode(const glm::ivec2& pin) const;
    bool isPosInsideOfNodeViewableArea(const glm::ivec2& pin) const;

    HkTransformBBox computeBBoxWith(const HkTransformContext& otherTc) const;


    /* Absolute transfrom of the mesh */
    glm::ivec2 scale, rot, pos;

private:
    HKAxisBoundsPoints computeXIntersectionPointsWith(const HkTransformContext& otherTc) const;
    HKAxisBoundsPoints computeYIntersectionPointsWith(const HkTransformContext& otherTc) const;

    void computeModelMatrix();
    glm::mat4 modelMat;

    /* Whats the actual viewable by user part of the mesh */
    glm::ivec2 vScale, vPos;
};
} // hkui