#include <cstdio> // FILE, fmemopen
#include "gif.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint32_t w, h;
    int delay = 100;
    size_t uniq_rgb_values;
    FILE *f;
    GifWriter g;

    // Do not want to work with inputs that are too small or aren't a multiple of 4
    if (size < 4096 || size % 4 != 0)
        return 0;

    //Assuming data is our RGBA8 stream, work backwards to find a logical width and height
    uniq_rgb_values = (size_t) size / 4;
    w = 16;
    h = (uint32_t) uniq_rgb_values / w;

    // Create an in-memory FILE
    uint8_t buff[size];
    memset(buff, 0, size);

    f = fmemopen(buff, size, "wb");

    GifBegin(&g, f, w, h, delay);
    GifWriteFrame(&g, data, w, h, delay);
    GifEnd(&g);

    return 0;
}
