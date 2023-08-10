#ifndef SPHERE_REMOVE_NODES_COMMAND_H
#define SPHERE_REMOVE_NODES_COMMAND_H

#include <vector>
#include <stdexcept>

#include "../node.h"
#include "command.h"


namespace sphere {

    class RemoveNodesCommand : Command {

    public:
        explicit RemoveNodesCommand(const std::vector<Node> &nodes) {

            // check if node is not root node
            for (auto const &node : nodes) {
                if (node.getParent() == nullptr) {
                    throw std::runtime_error("can't delete root node");
                }
            }
        }

        void undo() override {

        }

        void redo() override {

        }

    private:




    };
}

#endif //SPHERE_REMOVE_NODES_COMMAND_H
