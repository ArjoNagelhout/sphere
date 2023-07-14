#ifndef SPHERE_NODE_HPP
#define SPHERE_NODE_HPP

#include <string>
#include <utility>
#include <vector>

#include <glm/mat4x4.hpp>
#include<glm/gtc/quaternion.hpp>

namespace sphere {

    const int INDENTATION_AMOUNT = 4;

    /*
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

        /*
         * Adds a child to this node
         */
        void addChild(Node *node) {

            // make sure node is not this node
            // if (node == this) { throw std::runtime_error }
            children.push_back(node);
        }

        /*
         * Removes a child from this node
         */
        void removeChild(Node *node) {
            auto result = std::remove(children.begin(), children.end(), node);
        }

        /*
         * For reordering the hierarchy
         */
        void setChildIndex(Node *node, int index) {

        }

        // destroys this node
        void destroy() {
            // what happens with the node?
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

        /*
         * Gets parent of this node
         */
        Node *getParent() {

        };

        // TRANSFORM

        // operations

        /*
         * Performs a local rotation relative to the current rotation
         */
        void rotate(glm::vec3 eulerAngles) {

        }

        /*
         *
         */
        void rotate(glm::quat rotation) {

        }

        void translate() {

        }

        // directions in world space

        /*
         * Up directional vector in world space
         */
        glm::vec3 up() {

        }

        /*
         * Down directional vector in world space
         */
        glm::vec3 down() {

        }

        /*
         * Forward directional vector in world space
         */
        glm::vec3 forward() {

        }

        /*
         * Back directional vector in world space
         */
        glm::vec3 back() {

        }

        /*
         * Right directional vector in world space
         */
        glm::vec3 right() {

        }

        /*
         * Left directional vector in world space
         */
        glm::vec3 left() {

        }

        // getters

        glm::vec3 localPosition() {
            // last column of the matrix
            return {localMatrix[3]};
        }

        glm::quat localRotation() {
            return {0, 0, 0, 0};
        }

        glm::vec3 localEulerAngles() {
            return {0, 0, 0};
        }

        glm::vec3 localScale() {
            // length of first three column vectors
            return {0, 0, 0};
        }

        glm::vec3 position() {
            return {0, 0, 0};
        }

        glm::quat rotation() {
            return {0, 0, 0, 0};
        }

        glm::vec3 eulerAngles() {
            return {0, 0, 0};
        }

        glm::vec3 lossyScale() {
            return {0, 0, 0};
        }

        // setters

        void setLocalPosition(glm::vec3 localPosition) {
            localMatrix[3] = glm::vec4(localPosition.x, localPosition.y, localPosition.z, 1.0f);
        }

        void setLocalRotation(glm::quat localRotation) {

        }

        void setLocalEulerAngles(glm::vec3 localEulerAngles) {

        }

        void setLocalScale(glm::vec3 localScale) {

        }

        void setPosition(glm::vec3 position) {

        }

        void setRotation(glm::quat rotation) {

        }

        void setEulerAngles(glm::vec3 eulerAngles) {

        }

        void setLossyScale(glm::vec3 scale) {

        }

        // MATRICES

        /*
         *
         */
        glm::mat4x4 getLocalMatrix() {
            return localMatrix;
        }

        /*
         * Gets the (cached) computed world matrix
         * doesn't perform any operations
         */
        glm::mat4x4 getWorldMatrix() {
            return worldMatrix;
        }

        /*
         * Sets the local matrix so that it evaluates
         * the world matrix to the supplied world matrix
         */
        void setWorldMatrix(glm::mat4x4 worldMatrix) {
            this->worldMatrix = worldMatrix;
        }

        void setLocalMatrix(glm::mat4x4) {
            // propagate changes to all child nodes

            onWorldMatrixChanged();
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

        /*
         *
         */
        glm::mat4x4 localMatrix{};

        /*
         *
         */
        glm::mat4x4 worldMatrix{};

        /*
         *
         */
        void onWorldMatrixChanged() {

            //
            for (const auto &child : children) {

                //child

            }
        }
    };
}

#endif //SPHERE_NODE_HPP
