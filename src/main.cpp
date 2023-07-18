#include "application.hpp"

int main()
{
    sphere::Application application;

    try {
        application.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}