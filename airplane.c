#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main() {
    char* content;
    uint64_t size;
    readFile("airplane.json", &content, &size);
    printf("Size: %llu\n", size);
}
