//
// Created by Arjo Nagelhout on 2023-07-14.
//

#ifndef SPHERE_NODE_HPP
#define SPHERE_NODE_HPP

#include <string>
#include <utility>
#include <vector>

namespace sphere {

    const int INDENTATION_AMOUNT = 4;

    class Node {

    public:
        explicit Node(std::string name) {
            this->name = std::move(name);
        }

        // adds a child to this node
        void add(Node *node) {

            // make sure node is not this node
            // if (node == this) { throw std::runtime_error }
            children.push_back(node);
        }

        void remove(const Node& node) {
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

        void print(int maxLevels = 10) {
            print(0, maxLevels);
        }

        void print(int level, int maxLevels) {
            // first print self

            int spacesAmount = level * INDENTATION_AMOUNT;
            char *spacesBuffer = new char[spacesAmount];
            memset(spacesBuffer, ' ', sizeof(char) * spacesAmount);
            std::cout << spacesBuffer << "- " << name << "\n";
            delete[] spacesBuffer;

            int newLevel = level + 1;
            if (newLevel >= maxLevels) {
                return;
            }

            // then iterate over children and print them
            for (auto &child : children) {
                child->print(newLevel, maxLevels);
            }
        }
#pragma clang diagnostic pop

    private:
        std::string name;
        std::vector<Node *> children;
    };

}

#endif //SPHERE_NODE_HPP
