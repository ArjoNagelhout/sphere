#include "application.h"

#include <iostream>

int main()
{
    sphere::Application application("Sphere");

    try {
        application.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}