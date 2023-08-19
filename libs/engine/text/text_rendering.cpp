#include "text_rendering.h"

#include <stdexcept>
#include <iostream>

namespace engine {

    TextRendering::TextRendering() {

        auto error = FT_Init_FreeType( &library );
        if (error) {
            throw std::runtime_error("failed to initialize free type library");
        }

        std::cout << "initialized text rendering" << std::endl;
    }

    TextRendering::~TextRendering() = default;
}