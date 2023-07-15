#ifndef SPHERE_TRANSFORM_COMMAND_HPP
#define SPHERE_TRANSFORM_COMMAND_HPP

#include "command.hpp"
#include "../node.hpp"

#include <vector>
#include <glm/mat4x4.hpp>

namespace sphere {

    /*
     * Transforms a given set of objects
     */
    class TransformCommand : Command {

    public:

        struct TransformCommandData {
            Node *node;

            glm::vec3 newLocalPosition;
            glm::quat newLocalRotation;
            glm::vec3 newLocalScale;
        };

        explicit TransformCommand(const std::vector<TransformCommand::TransformCommandData> &transformCommandDataList) {

            // construct the internal transform command data

            for (auto const &transformCommandData : transformCommandDataList) {
                InternalTransformCommandData internalTransformCommandData{transformCommandData};
                this->data.push_back(internalTransformCommandData);
            }
        }

        void undo() override {
            for (auto const &transformCommandData : data) {
                Node *node = transformCommandData.node;

                node->setLocalPosition(transformCommandData.oldLocalPosition);
                node->setLocalRotation(transformCommandData.oldLocalRotation);
                node->setLocalScale(transformCommandData.oldLocalScale);
            }
        }

        void redo() override {
            for (auto const &transformCommandData : data) {
                Node *node = transformCommandData.node;

                node->setLocalPosition(transformCommandData.newLocalPosition);
                node->setLocalRotation(transformCommandData.newLocalRotation);
                node->setLocalScale(transformCommandData.newLocalScale);
            }
        }

    private:

        // data for the transform command per object
        struct InternalTransformCommandData {

            InternalTransformCommandData(const TransformCommandData &data)
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

        std::vector<InternalTransformCommandData> data{};
    };

}

#endif //SPHERE_TRANSFORM_COMMAND_HPP
