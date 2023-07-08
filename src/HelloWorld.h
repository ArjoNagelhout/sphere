#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class HelloWorld {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initVulkan() {
        std::cout << "Waza";
    }

    void mainLoop() {

    }

    void cleanup() {

    }
};