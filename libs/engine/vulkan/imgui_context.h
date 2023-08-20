#ifndef SPHERE_IMGUI_CONTEXT_H
#define SPHERE_IMGUI_CONTEXT_H

namespace engine {

    class ImguiContext {

    public:
        explicit ImguiContext();
        ~ImguiContext();

        std::function<void()> renderImgui;

    private:
        VkDescriptorPool imguiDescriptorPool;
    };
}

#endif //SPHERE_IMGUI_CONTEXT_H
