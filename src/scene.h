#ifndef SPHERE_SCENE_H
#define SPHERE_SCENE_H

#include <iostream>
#include <vector>
#include <string>

#include "node.h"

namespace sphere {

    class Scene {

    public:
        // constructor with name
        // uses initializer list
        explicit Scene(const std::string &name) : name(name) {
            std::cout << "Initialized scene \"" << name << "\"\n";
        }

        std::string getName() {
            return name;
        }

        // root node in the scene
        std::string rootString{"root"};
        Node rootNode{rootString, nullptr};

    private:

        // name of the scene
        std::string name;
    };
}

#endif //SPHERE_SCENE_H
