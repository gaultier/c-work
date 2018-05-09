#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int readFile(const char file_name[], char **str, uint64_t *size) {
    const int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file: %s (%d)\n", strerror(errno),
                errno);
        return -1;
    }

    struct stat s;
    const int res = fstat(fd, &s);
    if (res == -1) {
        fprintf(stderr, "Error fstat : %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    *size = (uint64_t)s.st_size;
    *str = mmap(NULL, *size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
    if (*str == MAP_FAILED) {
        fprintf(stderr, "Error mmapping file: %s (%d)\n", strerror(errno),
                errno);
        return -1;
    }

    return 0;
}
