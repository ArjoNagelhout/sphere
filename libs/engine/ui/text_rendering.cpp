#include "text_rendering.h"

#include <stdexcept>
#include <iostream>

namespace engine {

    static void checkFreetypeResult(FT_Error error) {
        if (error) {
            throw std::runtime_error(FT_Error_String(error));
        }
    }

    /*
     * https://freetype.org/freetype2/docs/tutorial/step1.html
     */
    TextRendering::TextRendering() {

        checkFreetypeResult(FT_Init_FreeType(&library));
        std::cout << "initialized text rendering" << std::endl;

        FT_Face face;

        std::string path = "/Users/arjonagelhout/Downloads/Staatliches/Staatliches-Regular.ttf";
        checkFreetypeResult(FT_New_Face(library, path.data(), 0, &face));

        std::cout << face->num_glyphs << std::endl;

        checkFreetypeResult(FT_Set_Pixel_Sizes(face, 64, 64));

        // 1 bit can represent 2 numbers
        // 8 bits can represent 2^32 = 4.294.967.296 numbers
        // 1 hex can represent 16 numbers
        // amount of hex required = log(2^(amount of bits)) / log(16) = log(2^32)/log(16) = 8

        // FT_Get_Char_Index only accepts UTF-32
        unsigned long charCode = 0x00000077; // 32 bits, UTF-32
        // returns no error code, returns 0, which corresponds to the "missing glyph", i.e. a box or space
        FT_UInt charIndex = FT_Get_Char_Index(face, charCode);

        checkFreetypeResult(FT_Load_Glyph(face, charIndex, 0));

    }

    TextRendering::~TextRendering() = default;
}