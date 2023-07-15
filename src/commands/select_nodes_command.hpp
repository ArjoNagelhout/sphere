#ifndef SPHERE_SELECT_NODES_COMMAND_HPP
#define SPHERE_SELECT_NODES_COMMAND_HPP

#include "command.hpp"
#include "../node.hpp"
#include "../selection.hpp"

namespace sphere {

    class SelectNodesCommand : Command {

    public:
        explicit SelectNodesCommand(const std::vector<Node> &nodes) {

        }

    private:


    };
}

#endif //SPHERE_SELECT_NODES_COMMAND_HPP
