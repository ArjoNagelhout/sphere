#ifndef SPHERE_TRANSFORM_COMMAND_HPP
#define SPHERE_TRANSFORM_COMMAND_HPP

#include "command.hpp"
#include "../node.hpp"

#include <vector>

namespace sphere {



    /*
     * Transforms a given set of objects
     */
    class TransformCommand : Command {

    public:

        TransformCommand() {

        }

        void undo() override {

        }

        void redo() override {

        }

    private:

        // data for the transform command per object
        struct TransformCommandData {

        };

        std::vector<TransformCommandData> data;

        void initialize() override {

        }
    };

}

#endif //SPHERE_TRANSFORM_COMMAND_HPP
