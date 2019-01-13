#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int cmp(const void* a, const void* b) {
    const int64_t x = *(const int64_t*)a;
    const int64_t y = *(const int64_t*)b;
    if (x < y)
        return -1;
    else if (x > y)
        return 1;
    return 0;
}

int main() {
    int64_t x;
    int64_t* numbers = NULL;
    uint64_t numbers_len = 0;

    while (scanf("%lld\n", &x) == 1) {
        numbers = realloc(numbers, ++numbers_len);
        numbers[numbers_len - 1] = x;
    }

    qsort(numbers, numbers_len, sizeof(x), cmp);

    for (uint64_t i = 0; i < numbers_len; i++) printf("%lld\n", numbers[i]);
}
