#ifndef SPHERE_PIPELINE_BUILDER_H
#define SPHERE_PIPELINE_BUILDER_H

#include <vulkan/vulkan.h>

namespace engine {

    class PipelineBuilder {

    public:
        explicit PipelineBuilder();
        ~PipelineBuilder();

    private:
        VkPipelineCache pipelineCache;


    };
}

#endif //SPHERE_PIPELINE_BUILDER_H
