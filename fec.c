#include <errno.h>
#include <fcntl.h>
#include <khash.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef WITH_JMALLOC
#include <jemalloc/jemalloc.h>
#endif

int readFile(const char file_name[], char **str, uint64_t *size) {
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

KHASH_MAP_INIT_STR(str, uint64_t)

int main() {
    char *file = NULL;
    uint64_t file_size = 0;
    if (readFile("fec_short.txt", &file, &file_size) != 0) return errno;

    // #1: Total number of lines
    {
        struct timeval start;
        gettimeofday(&start, NULL);
        uint64_t line = 0;
        for (uint64_t i = 0; i < file_size; i++) line += (file[i] == '\n');
        printf("Line count: %llu\n", line);

        struct timeval end;
        gettimeofday(&end, NULL);
        printf("Time #1: %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 +
                                        (end.tv_usec - start.tv_usec) / 1000);
    }

    // #2 Print out the 432nd and 43243rd names (8th field).
    {
        struct timeval start;
        gettimeofday(&start, NULL);

        uint64_t line_count = 0;
        char *line = alloca(1024);
        char *orig_line = line;
        char *f = file;

        while (f < file + file_size) {
            if (f == file || *(f - 1) == '\n') {  // Start of line
                line_count++;
                if (line_count > 43243) break;

                char *next_eol = strchr(f, '\n');
                line = orig_line;
                memcpy(line, f, (next_eol - f));

                char *token;
                uint8_t token_count = 1;
                char full_name[100];
                while ((token = strsep(&line, "|")) != NULL) {
                    if (token_count == 8) {
                        strncpy(full_name, token, 99);
                        full_name[99] = '\0';
                        break;
                    }
                    token_count++;
                }
                if (line_count == 432 || line_count == 43243)
                    printf("Line %llu: %s\n", line_count, full_name);

                f = next_eol;
            } else
                f++;
        }
        struct timeval end;
        gettimeofday(&end, NULL);
        printf("Time #2: %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 +
                                        (end.tv_usec - start.tv_usec) / 1000);
    }

    // # 3: Count how many donations occurred in each month (5th field)
    {
        struct timeval start;
        gettimeofday(&start, NULL);
        khiter_t k;
        khash_t(str) *monthly_count = kh_init(str);

        char *line = alloca(1024);
        char *orig_line = line;
        char *f = file;
        char months[50][7];
        bzero(months, 50 * 7);
        uint64_t months_len = 0;

        while (f < file + file_size) {
            if (f == file || *(f - 1) == '\n') {  // Start of line
                char *next_eol = strchr(f, '\n');
                line = orig_line;
                memcpy(line, f, (next_eol - f));

                char *token;
                uint8_t token_count = 1;
                char month[7];
                while ((token = strsep(&line, "|")) != NULL) {
                    if (token_count == 5) {
                        strncpy(month, token, 6);
                        month[6] = '\0';
                        break;
                    }
                    token_count++;
                }

                int absent;
                k = kh_put(str, monthly_count, month, &absent);
                if (absent) {
                    memcpy(months[months_len], month, 7);
                    kh_key(monthly_count, k) = months[months_len];
                    kh_value(monthly_count, k) = 1;
                    months_len++;
                } else
                    kh_value(monthly_count, k)++;
                f = next_eol;
            } else
                f++;
        }

        const char *month;
        uint64_t count;
        kh_foreach(monthly_count, month, count,
                   printf("%s: %llu\n", month, count));
        struct timeval end;
        gettimeofday(&end, NULL);
        printf("Time #3: %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 +
                                        (end.tv_usec - start.tv_usec) / 1000);
    }

    // # 4: Most common first name in the data and how many times it occurs
    {
        struct timeval start;
        gettimeofday(&start, NULL);
        khiter_t k;
        khash_t(str) *names = kh_init(str);

        char *line = alloca(1024);
        char *orig_line = line;
        char *f = file;

        while (f < file + file_size) {
            if (f == file || *(f - 1) == '\n') {  // Start of line
                char *next_eol = strchr(f, '\n');
                line = orig_line;
                memcpy(line, f, (next_eol - f));

                char *token;
                uint8_t token_count = 1;
                char full_name[100];
                while ((token = strsep(&line, "|")) != NULL) {
                    if (token_count == 8) {
                        strncpy(full_name, token, 99);
                        full_name[99] = '\0';
                        break;
                    }
                    token_count++;
                }

                int absent;
                k = kh_put(str, names, full_name, &absent);
                if (absent) {
                    kh_key(names, k) = strdup(full_name);
                    kh_value(names, k) = 1;
                } else
                    kh_value(names, k)++;
                f = next_eol;
            } else
                f++;
        }

        const char *full_name, *most_common_full_name = NULL;
        uint64_t count, max_count = 0;
        kh_foreach(names, full_name, count, if (count > max_count) {
            max_count = count;
            most_common_full_name = full_name;
        });

        printf("%s: %llu\n", most_common_full_name, max_count);
        struct timeval end;
        gettimeofday(&end, NULL);
        printf("Time #4: %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 +
                                        (end.tv_usec - start.tv_usec) / 1000);
    }
#ifdef WITH_JMALLOC
    malloc_stats_print(NULL, NULL, "J");
#endif
}
