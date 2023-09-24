#include "HkRenderContext.hpp"

namespace hkui
{

HkRenderArch HkRenderContext::renderArch = {
    .vertices = {
        // POS 3F         TEX 2F
        0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        },
    .indices = {
        0, 1, 3,
        1, 2, 3
        }
};

/* Sets shader code to be used */
void HkRenderContext::setShaderSource(const std::string& vertSource, const std::string& fragSource, HkRenderStore* renderStore)
{
    shader_.setShaderSource(vertSource, fragSource, renderStore);
    //TODO: maybe new param like: archName?
    const std::string archName = "QUAD";

    const uint32_t storedVaoId = renderStore->archNameToVaoIdMap[vertSource + fragSource];
    if (storedVaoId == 0)
    {
        setupArch();
        renderStore->archNameToVaoIdMap[vertSource + fragSource] = vaoId_;
        std::cout << "Generated VAO id: " << vaoId_ << "\n";
    }
    else
    {
        vaoId_ = storedVaoId;
    }

    // if (!archCreated_)
    // {
    //     archCreated_ = true;
    // }
}

/* Actually render the mesh */
void HkRenderContext::render(const glm::mat4& projMat, const glm::mat4& modelMat, HkRenderStore& renderStore)
{
    /* Set always mandatory to have uniforms */
    shader_.setMatrix4("proj", projMat);
    shader_.setMatrix4("model", modelMat);

    /* Setup whatever user defined uniforms need to be set */
    if (colorUniformEnabled_)
    {
        shader_.setVec3f("color", styleContextInj_->getColor());
    }

    //TODO: At some point batching will be needed to avoid context switching
    /* Dont try to bind texture if theres none to bind. Bind is expensive */
    for (const auto& texInfo : texInfos_)
    {
        /* Shader needs to know the name of the uniform and the texture unit assigned for that uniform,
           More over, it needs that texture into to be bound.*/
        glActiveTexture(texInfo.texUnit);
        shader_.setInt(texInfo.texName, texInfo.texUnit - GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texInfo.texId);
    }

    if (renderStore.boundVaoId == 0)
    {
        glBindVertexArray(vaoId_);
        std::cout << "VAO " << vaoId_ << " is now bound\n";
        renderStore.boundVaoId = vaoId_;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //TODO: Maybe unbinding is not really needed, just like with VAO
    // glBindTexture(GL_TEXTURE_2D, 0);
}

//TODO: Shall be better separated in the future
/* Setup buffers with the currently set architecture */
void HkRenderContext::setupArch()
{
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &vaoId_);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(vaoId_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderArch.vertices.size(),
        &renderArch.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * renderArch.indices.size(),
        &renderArch.indices[0], GL_STATIC_DRAW);

    // attribID, dims, type, _, stride (how much to next vert), offset (where in buffer data for attrID starts)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void HkRenderContext::addTexture(const HkTextureInfo& texInfo)
{
    texInfos_.push_back(texInfo);
}

void HkRenderContext::setColorUniformEnabled(const bool value)
{
    colorUniformEnabled_ = value;
}

/* Return gateway to shader object */
HkShader& HkRenderContext::getShader()
{
    return shader_;
}

void HkRenderContext::injectStyleContext(HkStyleContext* styleContext)
{
    styleContextInj_ = styleContext;
}
} // hkui
