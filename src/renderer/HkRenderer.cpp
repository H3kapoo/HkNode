#include "HkRenderer.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
HkRenderArch HkRenderer::renderArch_ = {
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

/* Generate shader for V+F combination and return id or simply return id if shader was already compiled before */
int32_t HkRenderer::addShaderSourceToCache(const std::string& vertSource, const std::string& fragSource)
{
    const uint32_t storedShaderId = pathToShaderIdMap_[vertSource + fragSource];
    if (storedShaderId == 0)
    {
        int32_t generatedShaderId = shader_.loadShaderFromSource(vertSource, fragSource);
        if (generatedShaderId != -1)
        {
            pathToShaderIdMap_[vertSource + fragSource] = generatedShaderId;
            std::cout << "Generated shader id: " << generatedShaderId << "\n";
            return generatedShaderId;
        }
        else
        {
            std::cout << "Failed to generate shader id for: " << vertSource << " + " << fragSource << "\n";
        }
    }
    return storedShaderId;
}

int32_t HkRenderer::addVertexArrayDataToCache(const std::string& archName) //TODO: Shall be enum
{
    // const std::string archName = "QUAD";
    const uint32_t storedVaoId = archNameToVaoIdMap_[archName];
    if (storedVaoId == 0)
    {
        //TODO: Switch case here for the different arch types in the future
        const uint32_t vaoId = setupQuadArch();
        archNameToVaoIdMap_[archName] = vaoId;
        std::cout << "Generated VAO id: " << vaoId << "\n";
        return vaoId;
    }
    return storedVaoId;
}

/* Actually render the mesh */
void HkRenderer::render(const HkRenderConfig& renderConfig, const HkStyleContext& styleConfig, const glm::mat4& modelMat)
{
    /* Set always mandatory to have uniforms */
    if (boundShaderId_ != renderConfig.shaderId)
    {
        shader_.bindId(renderConfig.shaderId);
        shader_.setMatrix4("proj", renderConfig.windowProjMatrix);
        shader_.setMatrix4("model", modelMat);
    }

    /* Setup whatever user defined uniforms need to be set */
    if (renderConfig.colorUniformEn)
    {
        shader_.setVec3f("color", styleConfig.getColor());
    }

    //TODO: At some point batching will be needed to avoid context switching
    /* Dont try to bind texture if theres none to bind. Bind is expensive */
    // for (const auto& texInfo : renderConfig.texIds)
    // {
    //     /* Shader needs to know the name of the uniform and the texture unit assigned for that uniform,
    //        More over, it needs that texture into to be bound.*/
    //     glActiveTexture(texInfo.texUnit);
    //     shader_.setInt(texInfo.texName, texInfo.texUnit - GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, texInfo.texId);
    // }

    if (boundVaoId_ != renderConfig.vaoId)
    {
        glBindVertexArray(renderConfig.vaoId);
        // std::cout << "VAO " << vaoId_ << " is now bound\n";
        boundVaoId_ = renderConfig.vaoId;
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //TODO: Maybe unbinding is not really needed, just like with VAO
    // glBindTexture(GL_TEXTURE_2D, 0);
}
/* Setup buffers with the currently set architecture */
int32_t HkRenderer::setupQuadArch()
{
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * renderArch_.vertices.size(),
        &renderArch_.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * renderArch_.indices.size(),
        &renderArch_.indices[0], GL_STATIC_DRAW);

    // attribID, dims, type, _, stride (how much to next vert), offset (where in buffer data for attrID starts)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}
} // hkui
