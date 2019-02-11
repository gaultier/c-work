// cc % -ljemalloc && ./a.out 100
// Prints the number of allocated bytes with jemalloc
#include <jemalloc/jemalloc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    size_t count = strtoll(argv[1], NULL, 10);

    size_t* numbers = calloc(1, count);

    size_t allocated = 0;
    size_t allocated_len = sizeof(allocated);
    mallctl("stats.allocated", &allocated, &allocated_len, NULL, 0);

    printf("Allocated: %zu\n", allocated);
}
