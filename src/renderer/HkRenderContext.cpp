#include "HkRenderContext.hpp"

namespace hkui
{

HkRenderContext::HkRenderContext()
    : shader_("assets/shaders/v1.glsl", "assets/shaders/f1.glsl")
{
    //TODO: This should be changable, hardcoded for now as we dont need another arch right now
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
    renderArch = rectangleArch;
}

/* Sets shader code to be used */
void HkRenderContext::setShaderSource(const std::string& vertSource, const std::string& fragSource)
{
    shader_.setShaderSource(vertSource, fragSource);
    setupArch();
}

/* Actually render the mesh */
void HkRenderContext::render(const glm::mat4& projMat, const glm::mat4& modelMat)
{
    shader_.bind();
    shader_.setMatrix4("proj", projMat);
    shader_.setMatrix4("model", modelMat);
    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/* Setup buffers with the currently set architecture */
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

/* Return gateway to shader object */
HkShader& HkRenderContext::getShader()
{
    return shader_;
}
} // hkui
