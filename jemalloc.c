// cc % -ljemalloc && ./a.out 100
// Prints stats with jemalloc
#include <errno.h>
#ifdef __FreeBSD__
#include <malloc_np.h>
#else
#include <jemalloc/jemalloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;

    size_t count = strtoll(argv[1], NULL, 10);

    size_t* numbers = malloc(count * sizeof(size_t));
    if (numbers == NULL) {
        fprintf(stderr, "Could not allocate: %d %s\n", errno, strerror(errno));
        exit(errno);
    }
    for (size_t i = 0; i < count; i++) numbers[i] = rand();

    size_t epoch = 1;
    size_t size = sizeof(size_t);
    {
        mallctl("epoch", NULL, NULL, &epoch, size);

        size_t allocated = 0;
        size_t allocated_len = sizeof(allocated);
        mallctl("stats.allocated", &allocated, &allocated_len, NULL, 0);
        printf("allocated: %zu\n", allocated);

        size_t active = 0;
        size_t active_len = sizeof(active);
        mallctl("stats.active", &active, &active_len, NULL, 0);
        printf("active: %zu\n", active);

        size_t mapped = 0;
        size_t mapped_len = sizeof(mapped);
        mallctl("stats.mapped", &mapped, &mapped_len, NULL, 0);
        printf("mapped: %zu\n", mapped);

        size_t retained = 0;
        size_t retained_len = sizeof(retained);
        mallctl("stats.retained", &retained, &retained_len, NULL, 0);
        printf("retained: %zu\n", retained);

        size_t resident = 0;
        size_t resident_len = sizeof(resident);
        mallctl("stats.resident", &resident, &resident_len, NULL, 0);
        printf("resident: %zu\n", resident);
    }

    size_t new_count = count * 2;
    numbers = realloc(numbers, new_count * sizeof(size_t));
    for (size_t i = 0; i < new_count; i++) numbers[i] = rand();

    {
        epoch += 1;
        mallctl("epoch", &epoch, &size, &epoch, size);

        size_t allocated = 0;
        size_t allocated_len = sizeof(allocated);
        mallctl("stats.allocated", &allocated, &allocated_len, NULL, 0);
        printf("allocated: %zu\n", allocated);

        size_t active = 0;
        size_t active_len = sizeof(active);
        mallctl("stats.active", &active, &active_len, NULL, 0);
        printf("active: %zu\n", active);

        size_t mapped = 0;
        size_t mapped_len = sizeof(mapped);
        mallctl("stats.mapped", &mapped, &mapped_len, NULL, 0);
        printf("mapped: %zu\n", mapped);

        size_t retained = 0;
        size_t retained_len = sizeof(retained);
        mallctl("stats.retained", &retained, &retained_len, NULL, 0);
        printf("retained: %zu\n", retained);

        size_t resident = 0;
        size_t resident_len = sizeof(resident);
        mallctl("stats.resident", &resident, &resident_len, NULL, 0);
        printf("resident: %zu\n", resident);
    }
}
