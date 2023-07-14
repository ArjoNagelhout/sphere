#ifndef SPHERE_SCENE_HPP
#define SPHERE_SCENE_HPP

#include <iostream>
#include <vector>
#include <string>

namespace sphere {

    class Scene {

    public:
        // constructor with name
        Scene(std::string name) {
            std::cout << "Initialized scene" << std::endl;
            this->name = name;
        }

        // adds an object to the scene
        void add(std::string object) {

        }

        // removes an object from the scene
        void remove(std::string object) {

        }

        std::string GetName() {
            return name;
        }

    private:

        // name of the scene
        std::string name;

        // list of objects in the scene
        std::vector<std::string> objects;

    };
}

#endif //SPHERE_SCENE_HPP
