#include "time.h"

#include <inttypes.h>
#include <stddef.h>
#include <sys/time.h>

uint32_t time_us() {
  struct timeval tv;
  int ret = gettimeofday(&tv, NULL);
  return 1000000 * tv.tv_sec + tv.tv_usec;
}
