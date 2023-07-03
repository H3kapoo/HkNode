#pragma once

#include <utility>
#include <stdint.h>
#include <iostream>

namespace hkui
{
    class HkInput
    {
    public:
        static void resizeEvent(uint32_t windowId, int width, int height)
        {
            std::cout << "resized\n";
        }

        // static std::pair<int, int> gets() { return std::pair<int, int>(windowWidth, windowHeight); }
    private:
        static int rWindowHeight;
        static int rWindowWidth;
    };

} // hkui