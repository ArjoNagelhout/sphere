#include "pipeline_builder.h"

namespace engine {

    PipelineBuilder::PipelineBuilder() {

        VkPipelineCacheCreateInfo pipelineCacheInfo{};
        pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipelineCacheInfo.pNext = nullptr;
        //pipelineCacheInfo.initialDataSize
        //pipelineCacheInfo.pInitialData
    }

    PipelineBuilder::~PipelineBuilder() {

    }

}