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
        explicit Scene(std::string name) : rootNode("root") {
            std::cout << "Initialized scene \"" << name << "\"\n";
            this->name = name;
        }

        std::string getName() {
            return name;
        }

        // root node in the scene
        Node rootNode;

    private:

        // name of the scene
        std::string name;
    };
}

#endif //SPHERE_SCENE_HPP
