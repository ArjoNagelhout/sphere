#include <iostream>

#include "HelloWorld.h"

//static void glfwError(int id, const char* description)
//{
//    std::cout << description << std::endl;
//}

int main()
{
    HelloWorld app;

    try{
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

//    GLFWwindow* window;
//
//    glfwSetErrorCallback(&glfwError);
//    if (!glfwInit())
//    {
//        exit(EXIT_FAILURE);
//    }
//
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    window = glfwCreateWindow(640, 480, "Sphere", nullptr, nullptr);
//    if (!window)
//    {
//        glfwTerminate();
//        exit(EXIT_FAILURE);
//    }
//
//    uint32_t extensionCount = 0;
//    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//    std::cout << extensionCount << " extensions supported\n";
//
//    glm::mat4 matrix;
//    glm::vec4 vec;
//    auto test = matrix * vec;
//
//    while (!glfwWindowShouldClose(window))
//    {
//        glfwPollEvents();
//    }
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//
//    exit(EXIT_SUCCESS);
}
