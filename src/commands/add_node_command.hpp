#ifndef SPHERE_ADD_NODE_COMMAND_HPP
#define SPHERE_ADD_NODE_COMMAND_HPP

#include "command.hpp"
#include "../scene.hpp"

namespace sphere {

    /*
     * Command for adding a node to a given scene
     */
    class AddNodeCommand : Command {

    public:
        AddNodeCommand(Scene &scene) : scene(&scene) {

        }



    private:
        Scene* scene;

    };
}

#endif //SPHERE_ADD_NODE_COMMAND_HPP
