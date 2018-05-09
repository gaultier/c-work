#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "utils.h"

typedef struct {
    const uint32_t str_i : 24, size : 8;
} String;

typedef struct {
    const uint32_t count;
    String strings[];
} Strings;

int main(int argc, const char* argv[]) {
    if (argc != 2) return 0;
    char* content;
    uint64_t size;
    int res = readFile(argv[1], &content, &size);
    if (res == -1) {
        fprintf(stderr, "[1] Error reading file: %s\n", strerror(errno));
        exit(1);
    }
    printf("[R000] %llu\n", size);

    const int fd_read = open("words.bin", O_RDONLY);

    struct stat st;
    const int res_st = fstat(fd_read, &st);
    if (res_st == -1) {
        fprintf(stderr, "Error fstat : %s\n", strerror(errno));
        return 1;
    }

    const uint32_t strings_size = st.st_size;
    printf("[R001] %u\n", strings_size);

    Strings* strings = calloc(1, strings_size);
    const int res_read = read(fd_read, strings, strings_size);
    if (res_read == -1 || res_read != strings_size)
        fprintf(stderr, "[2] Error reading file: %s\n", strerror(errno));
    printf("[R002] %u\n", strings->count);
    const String* s = &(strings->strings[strings->count - 1]);
    const char* word = &content[s->str_i];
    for (int k = 0; k < s->size; k++) {
        printf("%c", word[k]);
    }
    printf("\n");
}
