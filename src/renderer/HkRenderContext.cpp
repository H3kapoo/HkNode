#include "HkRenderContext.hpp"

namespace hkui
{

HkRenderContext::HkRenderContext()
    : renderArch(rectangleArch)
    , shader("assets/shaders/v1.glsl", "assets/shaders/f1.glsl") {}

void HkRenderContext::setShaderSource(const std::string& vertSource, const std::string& fragSource)
{
    // projMat = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, 0.0f, 100.0f);
    shader.setShaderSource(vertSource, fragSource);
    setupArch();
}

void HkRenderContext::render(const glm::mat4& projMat, const glm::mat4& modelMat)
{
    shader.bind();
    shader.setMatrix4("proj", projMat);
    shader.setMatrix4("model", modelMat);
    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void HkRenderContext::setupArch()
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
} // hkui
