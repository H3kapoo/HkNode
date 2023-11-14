#include "HkDrawDebugger.hpp"

namespace hkui
{

HkDrawDebugger& HkDrawDebugger::get()
{
    static HkDrawDebugger instance;
    return instance;
}

void HkDrawDebugger::drawBuffer()
{
    for (const auto& el : buffer_)
    {
        draw10x10(el);
    }
    buffer_.clear();
}

//TODO: Bug: this fills buffer too quickly if function is called from the same place and there are more events from there in the same frame
void HkDrawDebugger::pushDraw10x10(const glm::ivec2 pos)
{
    buffer_.push_back(pos);
}

void HkDrawDebugger::setDefaultDebugShader()
{
    // setup shader
    const std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    const std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    int32_t shaderId = shader_.loadShaderFromSource(DEFAULT_VS, DEFAULT_FS);
    if (shaderId) { shader_.bindId(shaderId); }

    shader_.setVec3f("color", glm::vec3(0.1f, 0.0f, 1.0f));

    // setup buffers
    float vertices[] = {
        // POS 3F         TEX 2F
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &vaoId_);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(vaoId_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
        &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
        &indices[0], GL_STATIC_DRAW);

    // attribID, dims, type, _, stride (how much to next vert), offset (where in buffer data for attrID starts)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void HkDrawDebugger::setProjectionMatrix(const glm::mat4& projMatrix)
{
    projMatrix_ = projMatrix;
}

void HkDrawDebugger::draw10x10(const glm::ivec2 pos)
{
    glBindVertexArray(vaoId_);

    tc_.setPos(pos);
    tc_.setScale({ 3,3 });
    shader_.bind();
    shader_.setMatrix4("proj", projMatrix_);
    shader_.setMatrix4("model", tc_.getModelMatrix());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
} // hkui