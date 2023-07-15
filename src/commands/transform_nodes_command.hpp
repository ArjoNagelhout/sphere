#ifndef SPHERE_TRANSFORM_NODES_COMMAND_HPP
#define SPHERE_TRANSFORM_NODES_COMMAND_HPP

#include "command.hpp"
#include "../node.hpp"

#include <vector>
#include <glm/mat4x4.hpp>

namespace sphere {

    /*
     * Transforms a given set of objects
     */
    class TransformNodesCommand : Command {

    public:

        struct Data {
            Node *node;

            glm::vec3 newLocalPosition;
            glm::quat newLocalRotation;
            glm::vec3 newLocalScale;
        };

        explicit TransformNodesCommand(const std::vector<Data> &dataList) {

            // construct the internal transform command data

            for (auto const &data : dataList) {
                InternalData internalData{data};
                this->dataList.push_back(internalData);
            }
        }

        void undo() override {
            for (auto const &data : dataList) {
                Node *node = data.node;

                node->setLocalPosition(data.oldLocalPosition);
                node->setLocalRotation(data.oldLocalRotation);
                node->setLocalScale(data.oldLocalScale);
            }
        }

        void redo() override {
            for (auto const &data : dataList) {
                Node *node = data.node;

                node->setLocalPosition(data.newLocalPosition);
                node->setLocalRotation(data.newLocalRotation);
                node->setLocalScale(data.newLocalScale);
            }
        }

    private:

        // data for the transform command per object
        struct InternalData {

            InternalData(const Data &data)
                : node(data.node),
                newLocalPosition(data.newLocalPosition),
                newLocalRotation(data.newLocalRotation),
                newLocalScale(data.newLocalScale) {

                oldLocalPosition = node->getLocalPosition();
                oldLocalRotation = node->getLocalRotation();
                oldLocalScale = node-> getLocalScale();
            }

            Node *node;

            glm::vec3 oldLocalPosition;
            glm::vec3 newLocalPosition;

            glm::quat oldLocalRotation;
            glm::quat newLocalRotation;

            glm::vec3 oldLocalScale;
            glm::vec3 newLocalScale;
        };

        std::vector<InternalData> dataList{};
    };

}

#endif //SPHERE_TRANSFORM_NODES_COMMAND_HPP
