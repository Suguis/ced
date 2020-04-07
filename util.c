#include "util.h"
#include <stdlib.h>
#include <stdio.h>

void *safe_malloc(size_t n) {
  void* p = malloc(n);
  if (!p) {
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
  }
  return p;
}
