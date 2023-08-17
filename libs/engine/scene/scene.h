#ifndef SPHERE_SCENE_H
#define SPHERE_SCENE_H

#include <vector>

#include "object.h"

namespace engine {

    /*
     * A scene contains a list of objects that can be drawn to the screen,
     * they should be sorted / grouped by their shader / pipeline object
     */
    class Scene {

    public:
        explicit Scene();
        ~Scene();

        std::vector<Object> objects;

    private:

    };
}

#endif //SPHERE_SCENE_H