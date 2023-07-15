#ifndef SPHERE_SCENE_HPP
#define SPHERE_SCENE_HPP

#include <iostream>
#include <vector>
#include <string>

#include "node.hpp"

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

#endif //SPHERE_SCENE_HPP
