#include "HkFontLoader.hpp"

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
bool HkFontLoader::load(const std::string& fontPath, const HkTextConfig& config)
{
    if (config.renderMethod == HkTextRenderMethod::SDF)
    {
        std::cerr << "SDF font rendering not implemented yet\n";
        return false;
    }

    FT_Library ftLib;
    if (FT_Init_FreeType(&ftLib))
    {
        std::cerr << "FreeType lib failed to load\n";
        return false;
    }

    FT_Face ftFace;
    if (FT_New_Face(ftLib, fontPath.c_str(), 0, &ftFace))
    {
        std::cerr << "Failed to load font\n";
        return false;
    }

    // charMap_.clear();

    // FT_Set_Pixel_Sizes(ftFace, 256, 256);
    // FT_Set_Pixel_Sizes(ftFace, 32, 32);
    FT_Set_Pixel_Sizes(ftFace, 16, 16);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureArrayId_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId_);
    // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 32, 32, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 16, 16, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    for (uint8_t i = 0; i < 128; i++)
    {
        if (FT_Load_Char(ftFace, i, FT_LOAD_RENDER))
        {
            std::cerr << "Error loading char: " << (char)i << "\n";
            continue;
        }

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, int(i),
            ftFace->glyph->bitmap.width,
            ftFace->glyph->bitmap.rows,
            1, GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);


        // set texture options
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        HkChar ch = {
            .charIndex = i,
            .advance = ftFace->glyph->advance.x,
            .size = glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows),
            .bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top)
        };

        charMap_[i] = ch;
    }

    // maybe unbind
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

    return true;
}
} // hkui