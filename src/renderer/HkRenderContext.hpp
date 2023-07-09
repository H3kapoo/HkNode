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

class HkRenderContext
{
public:
    HkRenderContext()
        : renderArch(rectangleArch)
        , shader("assets/shaders/v1.glsl", "assets/shaders/f1.glsl") {}

    void setupArch()
    {
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

    void setShaderSource(const std::string& vertSource, const std::string& fragSource)
    {
        //TODO: Refactor
        // transform.scale = { 100, 100 };
        // transform.pos = { 100, 100 };
        // projMat = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 100.0f);
        // shader("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
        projMat = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, 0.0f, 100.0f);
        shader.setShaderSource(vertSource, fragSource);
        // computeModelMatrix();
        setupArch();
    }

    void render(const glm::mat4& modelMat)
    {
        shader.bind();
        shader.setMatrix4("proj", projMat);
        shader.setMatrix4("model", modelMat);
        glBindVertexArray(vaoId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    HkRenderArch renderArch;
    HkShader shader;
    uint32_t vaoId;
    glm::mat4 projMat;
    // shader
    // Texture
    // Pos
};

} // hkui