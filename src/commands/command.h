#ifndef SPHERE_COMMAND_H
#define SPHERE_COMMAND_H

#include <vector>

namespace sphere {

    // base command that is used to create commands that can be added to the UndoStack
    class Command {

    public:

        // undo the command
        virtual void undo();

        // execute the command
        virtual void redo();

    private:

        std::vector<Command> children;
    };

} // sphere

#endif //SPHERE_COMMAND_H
