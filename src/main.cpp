#include <iostream>
#include <GLFW/glfw3.h>

// We use glad to generate the appropriate functions https://github.com/Dav1dde/glad
// We use Vulkan, as it's more performant on iOS and macOS devices, and a more modern API.
// We use glfw for drawing the window context. 

int main() {
    std::cout << "Hello sphere!" << std::endl;
    return 0;
}
