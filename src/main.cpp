#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// We use Vulkan, as it's more performant on iOS and macOS devices, and a more modern API.
// We use glfw for drawing the window context.
// + OpenXR?

static void glfwError(int id, const char* description)
{
    std::cout << description << std::endl;
}

int main()
{
    GLFWwindow* window;

    glfwSetErrorCallback(&glfwError);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    std::cout << "Wee \n";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(640, 480, "Sphere Wee", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported weeee\n";

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //std::cout << "A";
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
