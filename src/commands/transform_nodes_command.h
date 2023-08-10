#ifndef SPHERE_TRANSFORM_NODES_COMMAND_H
#define SPHERE_TRANSFORM_NODES_COMMAND_H

#include "command.h"
#include "../node.h"

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
                node->setLocalTransform(data.oldLocalPosition,
                                        data.oldLocalRotation,
                                        data.oldLocalScale);
            }
        }

        void redo() override {
            for (auto const &data : dataList) {
                Node *node = data.node;
                node->setLocalTransform(data.newLocalPosition,
                                        data.newLocalRotation,
                                        data.newLocalScale);
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

#endif //SPHERE_TRANSFORM_NODES_COMMAND_H
