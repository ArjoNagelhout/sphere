#ifndef SPHERE_ADD_NODES_COMMAND_H
#define SPHERE_ADD_NODES_COMMAND_H

#include "command.h"
#include "../scene.h"

namespace sphere {

    /*
     * Command for adding nodes to given nodes
     */
    class AddNodesCommand : Command {

    public:
        AddNodesCommand(Scene &scene) : scene(scene) {

        }



    private:
        Scene& scene;

    };
}

#endif //SPHERE_ADD_NODES_COMMAND_H
