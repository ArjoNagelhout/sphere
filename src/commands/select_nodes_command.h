#ifndef SPHERE_SELECT_NODES_COMMAND_H
#define SPHERE_SELECT_NODES_COMMAND_H

#include "command.h"
#include "../node.h"
#include "../selection.h"

namespace sphere {

    class SelectNodesCommand : Command {

    public:
        explicit SelectNodesCommand(const std::vector<Node> &nodes) {

        }

    private:


    };
}

#endif //SPHERE_SELECT_NODES_COMMAND_H
