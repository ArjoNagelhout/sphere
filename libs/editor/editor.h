#ifndef SPHERE_EDITOR_H
#define SPHERE_EDITOR_H

#include "panels/project_browser.h"

namespace editor {

    class Editor {
    public:
        explicit Editor();
        ~Editor();

        // call this using a lambda
        void render();

    private:
        ProjectBrowser projectBrowser;
        int selectedObjectIndex = 0;

    };
}

#endif //SPHERE_EDITOR_H
