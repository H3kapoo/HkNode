#pragma once

#include <iostream>
#include <memory>

namespace hkui
{
    class HkRenderableNode
    {
    public:
        // shader
        // VAO
        // Texture
    };

    class HkNodeRenderer
    {
    public:
        static void renderNode(HkRenderableNode* node)
        {
            std::cout << "rendering node\n";
        }
    };
} // hkui