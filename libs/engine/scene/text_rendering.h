#ifndef SPHERE_TEXT_RENDERING_H
#define SPHERE_TEXT_RENDERING_H

#include <ft2build.h>
#include <freetype/freetype.h>

namespace engine {

    /*
     * The approach for text rendering is to enable the user to load .ttf files on runtime
     * and have the engine convert glyphs on demand into a Signed Distance Field (SDF) representation
     * on a texture atlas or texture array.
     *
     * SDF is required because we're building an engine that will have a lot of UI in VR. This rules out traditional
     * approaches of caching the glyphs in rasterized / bitmap format at a specific font size.
     *
     * https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
     *
     * Another approach of rendering the font is by parsing it into a vector representation and rendering that using the
     * method described in:
     *
     * https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-25-rendering-vector-art-gpu
     *
     * https://mattdesl.svbtle.com/drawing-lines-is-hard
     *
     * https://news.ycombinator.com/item?id=26463464
     * http://gameenginegems.com/gemsdb/index.php
     * GPU Gems 1, 2, 3
     *
     * http://sluglibrary.com
     *
     * The results can be cached on disk. If no entry exists on disk, the font file will be parsed and the
     * rendering will be performed.
     *
     * We start out by using the simplest implementation possible using an existing library, and gradually
     *
     */
    class TextRendering {



    };

}

#endif //SPHERE_TEXT_RENDERING_H