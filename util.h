#ifndef UTIL_H
#define UTIL_H

#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

#define STDIN_IS_TERMINAL() (isatty(fileno(stdin)))

void *safe_malloc(size_t n);

#endif // UTIL_H
