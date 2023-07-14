#ifndef SPHERE_SELECTION_HPP
#define SPHERE_SELECTION_HPP

#include "node.hpp"

#include <vector>

namespace sphere {

    class Selection {

    public:
        Selection() = default;

    private:

        std::vector<Node> nodes;

    };

}

#endif //SPHERE_SELECTION_HPP
