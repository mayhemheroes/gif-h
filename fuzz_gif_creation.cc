#include "gif.h"

// For fuzzer utilities from Google
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_DATA_SIZE (sizeof(uint8_t) * 256 * 256 * 4)

/** COURTESY OF github.com/google/security-research-pocs **/

uint8_t* buff = (uint8_t*) malloc(MAX_DATA_SIZE);

// Redirect stdout to /dev/null. Useful to ignore output from verbose fuzz
// target functions.
//
// Return 0 on success, -1 otherwise.
extern "C" int ignore_stdout(void);

// Delete the file passed as argument and free the associated buffer. This
// function is meant to be called on buf_to_file return value.
//
// Return 0 on success, -1 otherwise.
extern "C" int delete_file(const char *pathname);

// Write the data provided in buf to a new temporary file. This function is
// meant to be called by LLVMFuzzerTestOneInput() for fuzz targets that only
// take file names (and not data) as input.
//
// Return the path of the newly created file or NULL on error. The caller should
// eventually free the returned buffer (see delete_file).
extern "C" char *buf_to_file(const uint8_t *buf, size_t size);


extern "C" int delete_file(const char *pathname) {
  int ret = unlink(pathname);
  if (ret == -1) {
    warn("failed to delete \"%s\"", pathname);
  }

  free((void *)pathname);

  return ret;
}

extern "C" char *buf_to_file(const uint8_t *buf, size_t size) {
  char *pathname = strdup("/dev/shm/fuzz-XXXXXX");
  if (pathname == nullptr) {
    return nullptr;
  }

  int fd = mkstemp(pathname);
  if (fd == -1) {
    warn("mkstemp(\"%s\")", pathname);
    free(pathname);
    return nullptr;
  }

  size_t pos = 0;
  while (pos < size) {
    int nbytes = write(fd, &buf[pos], size - pos);
    if (nbytes <= 0) {
      if (nbytes == -1 && errno == EINTR) {
        continue;
      }
      warn("write");
      goto err;
    }
    pos += nbytes;
  }

  if (close(fd) == -1) {
    warn("close");
    goto err;
  }

  return pathname;

err:
  delete_file(pathname);
  return nullptr;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    uint32_t w, h;
    int delay = 100;
    size_t uniq_rgb_values;
    char *file_path;
    GifWriter g;

    // Do not want to work with inputs that are too small, larger than the buffer, or aren't a multiple of 4
    if (size < 4096 || size > MAX_DATA_SIZE || size % 4 != 0)
        return 0;

    //Assuming data is our RGBA8 stream, work backwards to find a logical width and height
    uniq_rgb_values = (size_t) size / 4;
    w = 16;
    h = (uint32_t) uniq_rgb_values / w;

    // Create a temporary file to pass to GifBegin containing the fuzzer data
    file_path = buf_to_file(data, size);

    GifBegin(&g, file_path, w, h, delay);
    GifWriteFrame(&g, data, w, h, delay);
    GifEnd(&g);

    // Cleanup
    delete_file(file_path);

    return 0;
}
