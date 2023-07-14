#ifndef SPHERE_NODE_HPP
#define SPHERE_NODE_HPP

#include <string>
#include <utility>
#include <vector>

#include <glm/mat4x4.hpp>

namespace sphere {

    const int INDENTATION_AMOUNT = 4;

    /**
     * A node has a transform, a parent
     *
     * Events can be trickle down (from parent to child), or bubble up (from child to parent)
     *
     * Trickle down is used when transforms change.
     */
    class Node {

    public:
        explicit Node(std::string name) {
            this->name = std::move(name);
        }

        // NODE HIERARCHY

        // adds a child to this node
        void addChild(Node *node) {

            // make sure node is not this node
            // if (node == this) { throw std::runtime_error }
            children.push_back(node);
        }

        // removes a child from this node
        void removeChild(Node *node) {
            auto result = std::remove(children.begin(), children.end(), node);
        }

        // destroys this node
        void destroy() {

        }

        // sets the parent of this node to the given node
        void setParent(Node *node, bool worldPositionStays = false) {

            if (node == nullptr) {
                return;
            }

            if (node == this) {
                std::cout << "can't set node parent to itself" << std::endl;
                return;
            }

            // check if node is not child of this node
            // loop through all children, recursively traverse graph
        }

        /**
         * @brief Gets parent of this node
         * */
        Node *getParent{

        };

        // TRANSFORM

        glm::vec3 getLocalPosition() {
            return {localTransform[3]};
        }

        // todo
        glm::vec4 getLocalRotation() {
            return {};
        }

        glm::vec3 getLocalScale() {

        }

        glm::vec3 getPosition() {

        }

        glm::vec4 getRotation() {

        }

        glm::vec3 getScale() {

        }

        void setLocalPosition(glm::vec3 localPosition) {

        }

        void setLocalRotation() {

        }

        void setLocalScale(glm::vec3 localScale) {

        }

        void setPosition(glm::vec3 position) {

        }

        void setRotation() {

        }

        void setScale(glm::vec3 scale) {

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
            for (auto &child: children) {
                child->print(newLevel, maxLevels);
            }
        }

#pragma clang diagnostic pop

    private:
        std::string name;
        std::vector<Node *> children;

        // local transform that gets used to calculate the world transform
        glm::mat4x4 localTransform{};

        glm::mat4x4 worldTransform{};
    };
}

#endif //SPHERE_NODE_HPP
