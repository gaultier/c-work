#include <assert.h>
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

struct headers {
    size_t size;
    char** headers;
};

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

static char* make_str_from(const char* src, size_t len) {
    // TODO: maybe in case of empty str (len == 0), re-use the same static
    // string and bypass malloc?
    char* s = malloc(len + 1);
    memcpy(s, src, len + 1);
    s[len] = '\0';
    if (len >= 1 && s[len - 1] == '\r') s[len - 1] = '\0';
    return s;
}

static int on_header(const char* header, size_t header_len, size_t header_no,
                     void* data) {
    (void)header_len;
    (void)data;
    printf("Header: %zu: `%s`\n", header_no, header);

    struct headers* h = (struct headers*)(data);
    h->size += 1;
    h->headers = realloc(h->headers, sizeof(char*) * h->size);
    assert(header_no == (h->size - 1));
    h->headers[header_no] = strdup(header);
    return 0;
}

static int on_line(const char* line, size_t line_len, size_t line_no,
                   void* data) {
    (void)line_len;
    (void)data;
    printf("%zu: `%s`\n", line_no, line);
    return 0;
}

static int on_cell(const char* cell, size_t cell_len, size_t cell_no,
                   size_t line_no, void* data) {
    (void)cell_len;
    (void)data;
    struct headers* h = (struct headers*)(data);
    assert(cell_no < h->size);
    printf("%zu:%zu: `%s`: `%s`\n", line_no, cell_no, h->headers[cell_no],
           cell);
    return 0;
}

// TODO: validate ?
static int parse(const char file_name[], char sep,
                 int (*on_header)(const char* header, size_t header_len,
                                  size_t header_no, void* data),
                 int (*on_line)(const char* line, size_t line_len,
                                size_t line_no, void* data),
                 int (*on_cell)(const char* cell, size_t cell_len,
                                size_t cell_no, size_t line_no, void* data),
                 void* data) {
    char* file = NULL;
    uint64_t file_size = 0;
    if (read_file(file_name, &file, &file_size) != 0) return errno;

    const char* cur = file;
    size_t line_no = 0;
    size_t cell_no = 0;
    bool inside_quotes = false;
    const char* start_cell = file;
    const char* start_line = file;
    int ret = 0;

    while (cur < file + file_size) {
        // TODO: handle last line without newline
        // TODO handle "\r\n"
        if (*cur == '\n' && !inside_quotes) {
            {
                size_t cell_len = (size_t)(cur - start_cell);
                char* cell = make_str_from(start_cell, cell_len);
                // OPTIMIZE ?
                if (line_no == 0 && on_header != NULL)
                    ret = on_header(cell, cell_len, cell_no, data);
                else
                    ret = on_cell(cell, cell_len, cell_no, line_no, data);

                if (ret != 0) return ret;
                free(cell);
            }
            {
                size_t line_len = (size_t)(cur - start_line);
                char* line = make_str_from(start_line, line_len);

                if ((ret = on_line(line, line_len, line_no, data)) != 0)
                    return ret;

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
            size_t cell_len = (size_t)(cur - start_cell);
            char* cell = make_str_from(start_cell, cell_len);

            // OPTIMIZE ?
            if (line_no == 0 && on_header != NULL)
                ret = on_header(cell, cell_len, cell_no, data);
            else
                ret = on_cell(cell, cell_len, cell_no, line_no, data);

            if (ret != 0) return ret;
            free(cell);

            start_cell = cur + 1;
            cell_no++;
        }
        cur++;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s file separator\n", argv[0]);
        return 1;
    }

    const char* file = argv[1];
    const char* sep = argv[2];
    bool with_header = strlen(argv[3]) != 0;

    struct headers headers = {0};
    return parse(file, sep[0], with_header ? on_header : NULL, on_line, on_cell,
                 &headers);
}
