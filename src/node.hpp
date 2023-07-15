#ifndef SPHERE_NODE_HPP
#define SPHERE_NODE_HPP

#include <string>
#include <utility>
#include <vector>

#include <glm/mat4x4.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtx/quaternion.hpp>

namespace sphere {

    const int INDENTATION_AMOUNT = 4;

    /*
     * A node has a transform, and should always have a parent.
     *
     *
     *
     * Events can be trickle down (from parent to child), or bubble up (from child to parent)
     *
     * Trickle down is used when transforms change.
     */
    class Node {

    public:
        explicit Node(std::string name, Node *parent = nullptr)
            : name(std::move(name)), parent(parent) {

            if (parent != nullptr) {
                parent->addChild(this);
            }
        }

        /*
         * For reordering the hierarchy
         */
        void setChildIndex(Node *node, int index) {

        }

        /*
         * Destroy this node and all its children
         */
        void destroy() {

            // first destroy all child nodes

            // then destroy this node
        }

        // sets the parent of this node to the given node
//        void setParent(Node node, bool worldPositionStays = false) {
//
//            if (&node == nullptr) {
//                std::cout << "node is null" << std::endl;
//                return;
//            }
//
//            if (&node == this) {
//                std::cout << "can't set node parent to itself" << std::endl;
//                return;
//            }
//
//            // check if node is not child of this node
//            // loop through all children, recursively traverse graph
//        }

        /*
         * Gets pointer to parent of this node
         */
        Node *getParent() {
            return parent;
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
            return {0, 0, 0};
        }

        /*
         * Down directional vector in world space
         */
        glm::vec3 down() {
            return {0, 0, 0};
        }

        /*
         * Forward directional vector in world space
         */
        glm::vec3 forward() {
            return {0, 0, 0};
        }

        /*
         * Back directional vector in world space
         */
        glm::vec3 back() {
            return {0, 0, 0};
        }

        /*
         * Right directional vector in world space
         */
        glm::vec3 right() {
            return {0, 0, 0};
        }

        /*
         * Left directional vector in world space
         */
        glm::vec3 left() {
            return {0, 0, 0};
        }

        // getters

        glm::vec3 getLocalPosition() {
            return localPosition;
        }

        glm::quat getLocalRotation() {
            return localRotation;
        }

        glm::vec3 getLocalScale() {
            return localScale;
        }

        glm::vec3 getLocalEulerAngles() {
            return {0, 0, 0};
        }

        glm::vec3 getPosition() {
            // should transform the world space matrix with
            glm::vec4 positionColumn{localPosition, 1.0f};
            glm::vec4 result = computedWorldMatrix * positionColumn;
            return glm::vec3{result.x, result.y, result.z};
        }

        glm::quat getRotation() {
            return {0, 0, 0, 0};
        }

        glm::vec3 getEulerAngles() {
            return {0, 0, 0};
        }

        glm::vec3 getLossyScale() {
            return {0, 0, 0};
        }

        // setters

        void setLocalPosition(glm::vec3 position) {
            this->localPosition = position;
            recalculateLocalMatrix();
        }

        void setLocalRotation(glm::quat rotation) {
            this->localRotation = rotation;
            recalculateLocalMatrix();
        }

        void setLocalEulerAngles(glm::vec3 eulerAngles) {
            recalculateLocalMatrix();
        }

        void setLocalScale(glm::vec3 scale) {
            this->localScale = scale;
            recalculateLocalMatrix();
        }

        /*
         * Sets the world position
         */
        void setPosition(glm::vec3 position) {

        }

        /*
         * Sets the world rotation
         */
        void setRotation(glm::quat rotation) {

        }

        void setEulerAngles(glm::vec3 eulerAngles) {

        }

        void setLossyScale(glm::vec3 scale) {

        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

        void print(int maxLevels = 10) {
            print(0, maxLevels);
        }

        void print(int level, int maxLevels) {
            // first print self

            int spacesAmount = level * INDENTATION_AMOUNT;
            std::cout << std::string(spacesAmount, ' ') << "- " << name << "\n";

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
         * Should be set when the parent is changed using setParent
         */
        Node *parent;

        // we store the local position, rotation and scale
        // so that we don't have to continuously decompose and recompose the matrix

        glm::vec3 localPosition{0, 0, 0};

        // default: identity quaternion
        glm::quat localRotation{1.0f, 0, 0, 0};

        // default: scale of 1
        glm::vec3 localScale{1.0f, 1.0f, 1.0f};

        /*
         * The computed local matrix that gets recalculated each time the local position, rotation or scale
         * gets updated.
         */
        glm::mat4x4 computedLocalMatrix{1.0f};

        /*
         *
         */
        glm::mat4x4 computedWorldMatrix{1.0f};

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
        void removeChild(Node &node) {
            auto result = std::remove(children.begin(), children.end(), &node);
        }

        /*
         * Recalculates the local matrix
         * Should be called when localPosition, localRotation or localScale get changed.
         */
        void recalculateLocalMatrix() {

            // compose the local matrix from the translate, rotate and scale matrix
            glm::mat4x4 translateMatrix{glm::translate(localPosition)};
            glm::mat4x4 rotateMatrix{glm::toMat4(localRotation)};
            glm::mat4x4 scaleMatrix{glm::scale(localScale)};

            computedLocalMatrix = translateMatrix * rotateMatrix * scaleMatrix;

            // when the local matrix is recalculated, we should also recalculate the world matrix
            recalculateWorldMatrix();
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
        /*
         * Recalculates the world matrix using the parent's world matrix,
         * calls recalculateWorldMatrix on all children as well.
         */
        void recalculateWorldMatrix() {
            glm::mat4x4 parentMatrix;

            // use the parent's world matrix
            if (parent == nullptr) {
                parentMatrix = glm::mat4x4(1.0f); // use identity matrix if root node / floating node
            } else {
                parentMatrix = parent->computedWorldMatrix;
            }

            computedWorldMatrix = computedLocalMatrix * parentMatrix;

            // now loop over the children
            for (auto const &child: children) {
                child->recalculateWorldMatrix();
            }
        }
#pragma clang diagnostic pop
    };
}

#endif //SPHERE_NODE_HPP
