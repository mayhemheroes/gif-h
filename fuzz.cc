#include <cstdio> // FILE, fmemopen
#include "gif.h"

#define MAX_DATA_SIZE = sizeof(uint8_t) * 256 * 256 * 4

uint8_t* buff = (uint8_t*) malloc(MAX_DATA_SIZE);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint32_t w, h;
    int delay = 100;
    size_t uniq_rgb_values;
    FILE *f;
    GifWriter g;

    // Do not want to work with inputs that are too small, larger than the buffer, or aren't a multiple of 4
    if (size < 4096 || size > MAX_DATA_SIZE || size % 4 != 0)
        return 0;

    //Assuming data is our RGBA8 stream, work backwards to find a logical width and height
    uniq_rgb_values = (size_t) size / 4;
    w = 16;
    h = (uint32_t) uniq_rgb_values / w;

    // Create an in-memory FILE
    f = fmemopen(buff, size, "wb");

    GifBegin(&g, f, w, h, delay);
    GifWriteFrame(&g, data, w, h, delay);
    GifEnd(&g);

    return 0;
}
