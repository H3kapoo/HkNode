#include <glm/glm.hpp>

namespace hkui
{
struct HkTextRenderGLConfig
{
    uint32_t shaderId{ 0 };
    uint32_t vaoId{ 0 };
    uint32_t texId{ 0 };
    glm::mat4 windowProjMatrix;
};

struct HkTextRenderData
{
    static const uint32_t MAX_BATCH_SIZE{ 400 };
    glm::vec3 color{ 1.0f };
    int32_t letterMap[MAX_BATCH_SIZE];
    glm::mat4 modelMap[MAX_BATCH_SIZE];
};

} // hkui
