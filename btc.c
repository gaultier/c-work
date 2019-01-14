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

// To continue parsing the line, return 1.
// To stop and go to the next line return 0.
// To stop parsing the file return -1;
static int line_cb(const char str[], uint64_t cell_number, const char line[]) {
    if (cell_number == 1) {
        char* end;
        time_t timestamp = strtoll(str, &end, 10);
        return (end && *end == '\0' && timestamp >= 1420070400);
    } else if (cell_number == 2) {
        if (strcmp(str, "NaN") == 0) return 0;
        printf("%s\n", line);
        return -1;
    } else
        return 0;
}

static int read_string_line_by_line(const char* str, uint64_t size,
                                    int (*fn)(const char cell[],
                                              uint64_t cell_number,
                                              const char line[])) {
    char* line = alloca(100);
    char* orig_line = line;
    const char* s = str;

    while (s < str + size) {
        if (s == str || *(s - 1) == '\n') {
            char* next_eol = strchr(s, '\n');
            // Move back the line pointer to the initial memory region
            line = orig_line;
            // Copy the line
            memcpy(line, s, next_eol - s);
            line[next_eol - s] = '\0';

            // We need an immutable copy for the callback because strsep
            // increments the line pointer (but does not modify the content
            // pointed at)
            char full_line[100];
            memcpy(full_line, s, next_eol - s);
            full_line[next_eol - s] = '\0';

            char* token;
            uint8_t token_count = 1;

            while ((token = strsep(&line, ",")) != NULL) {
                int res = fn(token, token_count, full_line);
                if (res == 0)
                    break;
                else if (res == -1)
                    return 0;
                token_count++;
            }
            s = next_eol;
        }
        s++;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;

    char* file = NULL;
    uint64_t file_size = 0;
    if (readFile(argv[1], &file, &file_size) != 0) return errno;

    read_string_line_by_line(file, file_size, line_cb);
}
