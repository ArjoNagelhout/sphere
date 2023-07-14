#ifndef SPHERE_COMMAND_HPP
#define SPHERE_COMMAND_HPP

#include <vector>

namespace sphere {

    // base command that is used to create commands that can be added to the UndoStack
    class Command {

    public:
        Command() {
            initialize();
        }

        // undo the command
        void undo() {

        }

        // execute the command
        void redo() {

        }

    private:

        // perform setup, e.g. setting private variables
        virtual void initialize() {

        }

        std::vector<Command> children;
    };

} // sphere

#endif //SPHERE_COMMAND_HPP
