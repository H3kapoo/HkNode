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

class HkTransformContext
{
public:
    HkTransformContext();

    /* Setters */
    void addPos(const glm::ivec2& pos);
    void setPos(const glm::ivec2& pos);

    void addScale(const glm::ivec2& scale);
    void setScale(const glm::ivec2& scale);

    void setVScale(const glm::ivec2& vScale);
    void setVPos(const glm::ivec2& vPos);

    /* Getters */
    const glm::ivec2& getPos() const;
    const glm::ivec2& getScale() const;

    const glm::ivec2& getVPos() const;
    const glm::ivec2& getVScale() const;

    const glm::mat4& getModelMatrix();

    /* Pub funcs */
    bool isPosInsideOfNode(const glm::ivec2& pin) const;
    bool isPosInsideOfNodeViewableArea(const glm::ivec2& pin) const;
    HkTransformBBox computeBBoxWith(const HkTransformContext& otherTc) const;
private:
    HKAxisBoundsPoints computeXIntersectionPointsWith(const HkTransformContext& otherTc) const;
    HKAxisBoundsPoints computeYIntersectionPointsWith(const HkTransformContext& otherTc) const;
    void computeModelMatrix();

    glm::mat4 modelMat;

    /* Absolute transfrom of the mesh */
    glm::ivec2 scale, rot, pos;

    /* Whats the actual viewable by user part of the mesh */
    glm::ivec2 vScale, vPos;
};
} // hkui