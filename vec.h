#pragma once

#include "stdlib.h"

#define vec_add(v, size, elem)                                                 \
  do {                                                                         \
    (v) = realloc(v, (size + 1) * sizeof(elem));                               \
    (v)[size] = (elem);                                                        \
    (size)++;                                                                  \
  } while (0)
