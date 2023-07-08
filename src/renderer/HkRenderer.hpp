#pragma once

#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <string>

#include "HkShader.hpp"

namespace hkui
{

struct HkRenderArch
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

const HkRenderArch rectangleArch = {
    .vertices = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f},
    .indices = {
        0, 1, 3,
        1, 2, 3}
};

struct HkTransform
{
    glm::vec2 pos, scale, rot;
};

class HkRenderableNode
{
public:
    HkRenderableNode(const std::string& vertPath, const std::string& fragPath)
        : shader(vertPath, fragPath), renderArch(rectangleArch)
    {
        //TODO: Refactor
        transform.scale = { 100, 100 };
        transform.pos = { 100, 100 };
        projMat = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 100.0f);
        computeModelMatrix();

        unsigned int VBO, EBO;
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderArch.vertices.size(),
            &renderArch.vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * renderArch.indices.size(),
            &renderArch.indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void addPos(const glm::vec2& pos)
    {
        transform.pos += pos;
        computeModelMatrix();
    }
    void setPos(const glm::vec2& pos)
    {
        transform.pos = pos;
        computeModelMatrix();
    }
    void computeModelMatrix() {
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(transform.pos, -1.0f)); // it goes negative, expected
        // modelMat = glm::translate(modelMat, glm::vec3(pivotPoint_, 0));
        // modelMat = glm::rotate(modelMat, glm::radians(glm::degrees(rot_)), glm::vec3(0, 0, 1));
        modelMat = glm::scale(modelMat, glm::vec3(transform.scale, 1.0f));
        // modelMat = glm::translate(modelMat, glm::vec3(-pivotPoint_, 0));
    }

    void render()
    {
        shader.bind();
        shader.setMatrix4("proj", projMat);
        shader.setMatrix4("model", modelMat);
        glBindVertexArray(vaoId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    virtual ~HkRenderableNode() = default;

    HkShader shader;
    HkTransform transform;
    HkRenderArch renderArch;
    uint32_t vaoId;
    glm::mat4 modelMat, projMat;
    // shader
    // Texture
    // Pos
};

class HkNodeRenderer
{
public:
    // no need to create smart pointer as passed ptr is already memory managed
    static void renderNode(HkRenderableNode*)
    {
        // node->shader.bind();
        // node->bindVaoAndDraw();
        std::cout << "rendering node\n";
    }
};
} // hkui