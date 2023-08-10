#ifndef SPHERE_RENDER_PASS_HPP
#define SPHERE_RENDER_PASS_HPP

#include "engine.hpp"

namespace renderer {

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
        explicit RenderPass(Engine &engine, const VkFormat &format);
        ~RenderPass();

    private:
        Engine &engine;
        VkRenderPass renderPass;
    };
}

#endif //SPHERE_RENDER_PASS_HPP
