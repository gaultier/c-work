#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int readFile(const char file_name[], char** str, uint64_t* size) {
    const int fd = open(file_name, O_RDONLY);
    if (fd == -1) return errno;

    struct stat s;
    const int res = fstat(fd, &s);
    if (res == -1) {
        close(fd);
        return errno;
    }

    *size = (uint64_t)s.st_size;
    *str = mmap(NULL, *size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
    if (*str == MAP_FAILED) {
        close(fd);
        return errno;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    char* file = NULL;
    uint64_t file_size = 0;
    if (readFile(argv[1], &file, &file_size) != 0) return errno;

    char* line = alloca(1024);
    char* orig_line = line;
    char* f = file;

    while (f < file + file_size) {
        if (f == file || *(f - 1) == '\n') {
            char* next_eol = strchr(f, '\n');
            line = orig_line;
            memcpy(line, f, (next_eol - f));

            char* token;
            uint8_t token_count = 1;
            time_t timestamp = 0;
            char* end = NULL;

            while ((token = strsep(&line, ",")) != NULL) {
                if (token_count == 1)
                    timestamp = strtoll(token, &end, 10);
                else if (token_count == 2 && strcmp(token, "NaN") == 0)
                    break;
                else if (token_count == 2) {
                    if (end && *end == '\0' && timestamp >= 1420070400) {
                        memcpy(line, f, next_eol - f);
                        line[next_eol - f] = '\0';
                        printf("%s", line);
                        return 0;
                    }
                } else if (token_count > 2)
                    break;

                token_count++;
            }
            f = next_eol;
        }
        f++;
    }
}
