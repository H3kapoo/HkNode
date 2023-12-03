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
    void addContentPos(const glm::ivec2& pos);
    void setContentPos(const glm::ivec2& pos);

    void addScale(const glm::ivec2& scale);
    void setScale(const glm::ivec2& scale);
    void addContentScale(const glm::ivec2& scale);
    void setContentScale(const glm::ivec2& scale);

    void setVScale(const glm::ivec2& vScale);
    void setVPos(const glm::ivec2& vPos);
    void setCVScale(const glm::ivec2& vScale);
    void setCVPos(const glm::ivec2& vPos);

    /* Getters */
    const glm::ivec2& getContentPos() const;
    const glm::ivec2& getContentScale() const;
    const glm::ivec2& getPos() const;
    const glm::ivec2& getScale() const;

    const glm::ivec2& getVPos() const;
    const glm::ivec2& getVScale() const;
    const glm::ivec2& getCVPos() const;
    const glm::ivec2& getCVScale() const;

    const glm::mat4& getModelMatrix();

    /* Pub funcs */
    bool isPosInsideOfNode(const glm::ivec2& pin) const;
    bool isPosInsideOfNodeViewableArea(const glm::ivec2& pin) const;
    static HkTransformBBox computeBBoxWith(const HkTransformBBox& leftTc,
        const HkTransformBBox& rightTc);
    bool isAnyDifference() const;
    void copyContentDataToAbsoluteData();

private:
    static HKAxisBoundsPoints computeXIntersectionPointsWith(const HkTransformBBox& leftTc,
        const HkTransformBBox& rightTc);
    static HKAxisBoundsPoints computeYIntersectionPointsWith(const HkTransformBBox& leftTc,
        const HkTransformBBox& rightTc);
    void computeModelMatrix();

    glm::mat4 modelMat;

    /* Absolute transform of the mesh. Includes margins+borders+padding */
    glm::ivec2 scale, rot, pos;

    /* Content transform of the mesh. Doesnt include anything but the viewable content area */
    glm::ivec2 cScale, cRot, cPos;

    /* Whats the actual viewable by user part of the mesh */
    glm::ivec2 vScale, vPos;

    /* Whats the content viewable by user part of the mesh */
    glm::ivec2 cvScale, cvPos;

    /* Is matrix dirty? */
    glm::ivec2 prevScale, prevPos;
    bool isDirty_{ true };
};
} // hkui