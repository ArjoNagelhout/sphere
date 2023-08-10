#ifndef SPHERE_SELECTION_H
#define SPHERE_SELECTION_H

#include "node.h"

#include <vector>

namespace sphere {

    class Selection {

    public:
        Selection() = default;

    private:

        std::vector<Node *> nodes;

    };

}

#endif //SPHERE_SELECTION_H
