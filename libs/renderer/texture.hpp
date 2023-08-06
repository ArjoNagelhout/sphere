#ifndef SPHERE_TEXTURE_HPP
#define SPHERE_TEXTURE_HPP

#include <vk_mem_alloc.h>

namespace renderer {

    /*
     * A Texture that is present on the GPU
     */
    class Texture {

        explicit Texture() {
            // create buffer
        }

        ~Texture() {
            // deallocate buffer
        }
    };

}

#endif //SPHERE_TEXTURE_HPP
