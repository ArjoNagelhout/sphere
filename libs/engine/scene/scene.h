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

        void addObject(Object object) {}
        void removeObject(Object object) {}

    private:
        std::vector<Object> objects;
    };
}

#endif //SPHERE_SCENE_H