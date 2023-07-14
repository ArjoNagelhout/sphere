#ifndef SPHERE_SCENE_HPP
#define SPHERE_SCENE_HPP

#include <iostream>
#include <vector>
#include <string>

#include "node.hpp"

namespace sphere {

    const char* ROOT_NODE_NAME = "root";

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
        Node rootNode{ROOT_NODE_NAME, nullptr};

    private:

        // name of the scene
        std::string name;
    };
}

#endif //SPHERE_SCENE_HPP
