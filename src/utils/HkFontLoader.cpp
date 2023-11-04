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
    if (FT_New_Face(ftLib, "assets/fonts/LiberationSerif-Regular.ttf", 0, &ftFace))
    {
        std::cerr << "Failed to load font\n";
        return false;
    }

    charMap_.clear();

    FT_Set_Pixel_Sizes(ftFace, 0, 24);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (uint8_t i = 0; i < 128; i++)
    {
        if (FT_Load_Char(ftFace, i, FT_LOAD_RENDER))
        {
            std::cerr << "Error loading char: " << (char)i << "\n";
            continue;
        }

        uint32_t textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            ftFace->glyph->bitmap.width,
            ftFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            ftFace->glyph->bitmap.buffer);

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        HkChar ch = {
            .textureId = textureId,
            .advance = ftFace->glyph->advance.x,
            .size = glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows),
            .bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top)
        };

        charMap_[i] = ch;
    }

    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

    return true;
}
} // hkui