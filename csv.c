#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int read_file(const char file_name[], char** str, uint64_t* size) {
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

void on_line(const char* line, size_t line_len, size_t line_no) {
    printf("Line %zu: `%s`\n", line_no, line);
}

void on_cell(const char* cell, size_t cell_len, size_t cell_no) {
    printf("Cell %zu: `%s`\n", cell_no, cell);
}

static int parse(const char file_name[], char sep,
                 void (*on_line)(const char* line, size_t line_len,
                                 size_t line_no),
                 void (*on_cell)(const char* cell, size_t cell_len,
                                 size_t cell_no)) {
    char* file = NULL;
    uint64_t file_size = 0;
    if (read_file(file_name, &file, &file_size) != 0) return errno;

    const char* cur = file;
    size_t line_no = 1;
    size_t cell_no = 1;
    bool inside_quotes = false;
    const char* start_cell = file;
    const char* start_line = file;

    while (cur < file + file_size) {
        if (*cur == '\n' && !inside_quotes) {
            {
                size_t cell_len = cur - start_cell;
                char* cell = malloc(cell_len + 1);
                memcpy(cell, start_cell, cell_len + 1);
                cell[cell_len] = '\0';

                on_cell(cell, cell_len, cell_no);
                free(cell);
            }
            {
                size_t line_len = cur - start_line;
                char* line = malloc(line_len + 1);
                memcpy(line, start_line, line_len);
                line[line_len] = '\0';

                on_line(line, line_len, line_no);

                free(line);
            }
            start_line = cur + 1;
            start_cell = cur + 1;
            line_no++;
            cell_no = 0;
        } else if (*cur == '"') {
            if ((cur < file + file_size - 1) && *(cur + 1) != '"')
                inside_quotes = !inside_quotes;
        } else if (*cur == sep && !inside_quotes) {
            size_t cell_len = cur - start_cell;
            char* cell = malloc(cell_len + 1);
            memcpy(cell, start_cell, cell_len + 1);
            cell[cell_len] = '\0';

            on_cell(cell, cell_len, cell_no);
            free(cell);
            start_cell = cur + 1;
            cell_no++;
        }
        cur++;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("Usage: %s file separator cmd line column\n", argv[0]);
        return 1;
    }

    const char* file = argv[1];
    const char* sep = argv[2];
    const char* cmd = argv[3];
    if (strcmp(cmd, "print_cell") == 0)
        return parse(file, sep[0], on_line, on_cell);
}
