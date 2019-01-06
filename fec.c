#include <errno.h>
#include <fcntl.h>
#include <khash.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

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

int main(int argc, char *argv[]) {
    char *file = NULL;
    uint64_t file_size = 0;
    if (readFile("fec.dataset.txt", &file, &file_size) != 0) return errno;

    // #1: Total number of lines
    if (strcmp(argv[1], "1") == 0) {
        uint64_t line = 0;
        for (uint64_t i = 0; i < file_size; i++) line += (file[i] == '\n');
        printf("Line count: %llu\n", line);
    }

    // #2 Print out the 432nd and 43243rd names (8th field).
    if (strcmp(argv[1], "2") == 0) {
        uint64_t line = 1;
        char *f = file;
        while (line <= 43243) {
            // If we are at the start of the interesting lines
            if ((line == 432 || line == 43243) && *(f - 1) == '\n') {
                // Then advance to the 7th separator i.e the beginning of the
                // 8th field i.e name
                for (uint64_t k = 1; k <= 7; k++)
                    while (*++f != '|')
                        ;

                f++;  // Swallow the trailing '|'

                // Print the name and advance
                while (*f != '|') putchar(*f++);
                putchar('\n');
            }

            if (*f == '\n') line++;
            f++;
        }
    }

    // # 3: Count how many donations occurred in each month (5th field)
    if (strcmp(argv[1], "3") == 0) {
        khiter_t k;
        khash_t(str) *monthly_count = kh_init(str);

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
                    kh_key(monthly_count, k) = strdup(month);
                    kh_value(monthly_count, k) = 0;
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
    }
}
