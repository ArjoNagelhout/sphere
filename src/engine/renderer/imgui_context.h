#ifndef SPHERE_IMGUI_CONTEXT_H
#define SPHERE_IMGUI_CONTEXT_H

#include "swapchain.h"

namespace engine::renderer {

    class ImguiContext {

    public:
        explicit ImguiContext(VkRenderPass renderPass, const Swapchain &swapchain);
        ~ImguiContext();

        std::function<void()> renderImgui;

    private:
        VkDescriptorPool imguiDescriptorPool;
    };
}

#endif //SPHERE_IMGUI_CONTEXT_H
