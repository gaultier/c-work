#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sds.c"
#include "sds.h"

// static int sumWithFscanf(char sep[], size_t target_line, size_t
// target_column,
//                         char file_name[]) {
//    FILE* file = fopen(file_name, "r");
//    if (!file) {
//        fprintf(stderr, "The file %s could not be opened: %d %s\n", file_name,
//                errno, strerror(errno));
//        return errno;
//    }
//    char* line = NULL;
//    int token_count = 0;
//    size_t line_no = 0;
//    size_t line_cap = 0;
//
//    int res = 0;
//    while (line_no < target_line && (res = getline(&line, &line_cap, file)) >
//    0)
//        line_no++;
//
//    if (res == -1 && errno != 0) {
//        fprintf(stderr,
//                "An error occurred while reading the file %s: %d %s %d\n",
//                file_name, errno, strerror(errno), res);
//        return errno;
//    }
//
//    if (line_no < target_line) {
//        fprintf(stderr, "The file only contains %zu lines\n", line_no);
//        return ERANGE;
//    }
//
//    sds* tokens = sdssplitlen(line, res, sep, strlen(sep), &token_count);
//    if (token_count < target_column) {
//        fprintf(stderr,
//                "The line %zu is malformed: does not contain at least %zu "
//                "columns\n",
//                line_no, target_column);
//        fprintf(stderr, "Line: %s", line);
//        return ERANGE;
//    }
//
//    const char* const token = tokens[target_column - 1];
//    printf("%zu:%zu: %s\n", target_line, target_column, token);
//
//    return 0;
//}

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

static int line_print_cell(sds line, size_t line_no, const char sep[],
                           size_t sep_len, size_t target_line,
                           size_t target_column) {
    int token_count = 0;
    sds* tokens =
        sdssplitlen(line, sdslen(line), sep, strlen(sep), &token_count);
    if (token_count < target_column) {
        fprintf(stderr,
                "The line %zu is malformed: does not contain at least %zu "
                "columns (contains %d)\n",
                line_no, target_column, token_count);
        return ERANGE;
    }

    const char* const token = tokens[target_column - 1];
    printf("%zu:%zu: %s\n", target_line, target_column, token);
    return 0;
}

static int line_count_seps(const char* line, size_t line_no, const char sep[],
                           size_t sep_len) {
    size_t count = 0;
    const char* sep_it = line;
    while ((sep_it = strstr(sep_it, sep))) count++, sep_it += sep_len;

    printf("%zu: %zu\n", line_no, count);
    return count;
}

static int count_seps(const char file_name[], const char sep[]) {
    char* file = NULL;
    uint64_t file_size = 0;
    if (read_file(file_name, &file, &file_size) != 0) return errno;

    const char* cur = file;
    size_t line_no = 0;

    while (cur < file + file_size) {
        const char* const eol = strchr(cur, '\n');
        size_t line_len = eol - cur;
        char* line = malloc(line_len + 1);
        memcpy(line, cur, line_len);
        line[line_len] = '\0';
        line_count_seps(line, line_no, sep, strlen(sep));
        free(line);

        cur = strchr(cur, '\n') + 1;
        line_no++;
    }

    return 0;
}

static int print_cell(const char file_name[], const char sep[],
                      size_t target_line, size_t target_column) {
    char* file = NULL;
    uint64_t file_size = 0;
    if (read_file(file_name, &file, &file_size) != 0) return errno;

    const char* cur = file;
    size_t line_no = 1;

    while (cur < file + file_size && line_no < target_line) {
        cur = strchr(cur, '\n') + 1;
        line_no++;
    }

    if (line_no < target_line) {
        fprintf(stderr, "The file only contains %zu lines\n", line_no);
        return ERANGE;
    }

    const char* const eol = strchr(cur, '\n');
    sds line = sdsnewlen(cur, eol - cur);
    return line_print_cell(line, line_no, sep, strlen(sep), target_line,
                           target_column);
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
        return print_cell(file, sep, strtoull(argv[4], NULL, 10),
                          strtoull(argv[5], NULL, 10));
    else if (strcmp(cmd, "count_seps") == 0)
        return count_seps(file, sep);
}
