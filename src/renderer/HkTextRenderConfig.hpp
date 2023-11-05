#include <glm/glm.hpp>

namespace hkui
{
struct HkTextRenderConfig
{
    glm::vec3 color{ 1.0f };
    uint32_t shaderId{ 0 };
    uint32_t vaoId{ 0 };
    uint32_t texId{ 0 };
    uint32_t amount{ 0 };
    glm::mat4 windowProjMatrix;
};
} // hkui
