#ifndef SPHERE_RENDER_PASS_H
#define SPHERE_RENDER_PASS_H

#include <vulkan/vulkan.h>

namespace engine::renderer {

    /*
     * A render pass defines a set of image resources (attachments) to be used during rendering.
     *
     * draw commands (vkCmdDraw...) need to be recorded into a render pass instance using vkCmdBeginRenderPass
     *
     * VkRenderPass represents collection of attachments, subpasses, dependencies between these subpasses, and
     * describes how attachments are handled between these subpasses.
     */
    class RenderPass {

    public:
        explicit RenderPass(const VkFormat &format, const VkFormat &depthFormat);
        ~RenderPass();

        VkRenderPass renderPass;

    private:

    };
}

#endif //SPHERE_RENDER_PASS_H
