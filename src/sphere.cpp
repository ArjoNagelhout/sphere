#include <iostream>

#include "sphere.hpp"

namespace editor {
    void Editor::printStrings() {
        for (auto const &string : strings) {
            std::cout << string << std::endl;
        }
    }
}