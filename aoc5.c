#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* memcpy_without_sub(size_t* dst_size, char* src, size_t src_size,
                         size_t sub_i, size_t sub_size) {
        const size_t new_size = src_size - sub_size;
        char* dst = calloc(*dst_size, 1);

        memcpy(dst, src, sub_i);
        memcpy(dst + sub_i, src + sub_i + sub_size,
               src_size - (sub_i + sub_size));

        *dst_size = new_size;
        return dst;
}

int main() {
        FILE* f = fopen("/Users/pgaultier/Downloads/aoc5.txt", "r");
        fseek(f, 0, SEEK_END);
        size_t string_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char* string = calloc(string_size + 1, 1);

        fread(string, 1, string_size, f);
        fclose(f);

        string[string_size] = 0;
        size_t i = string_size - 1;
        while (string[i] == ' ' || string[i] == '\n' || string[i] == '\t') {
                string[i] = 0;
                string_size--;
                i--;
        }

        printf("[D001] size=%ld\n", string_size);

        /* char src[] = {0, 1, 2, 3, 4, 5, 6}; */
        /* size_t src_size = sizeof(src); */
        /* memcpy_without_sub(&dst, &dst_size, src, src_size, 4, 2); */
        /* for (size_t i = 0; i < dst_size; i++) printf("%d\n", dst[i]); */

        for (size_t n = 0; n < string_size; n++) {
                size_t i = 0;
                while (i < string_size) {
                        const char a = string[i];
                        const char b = string[i + 1];
                        if (abs(a - b) == 32) {
                                string = memcpy_without_sub(
                                    &string_size, string, string_size, i, 2);
                                i = 0;
                                continue;
                        }
                        i++;
                }
        }

        for (size_t i = 0; i < string_size - 1; i++) {
                const char a = string[i];
                const char b = string[i + 1];
                if (abs(a - b) == 32) {
                        printf("Precondition failed: s[%zu]=%c s[%zu]=%c\n", i,
                               a, i + 1, b);
                        return 1;
                }
        }
        printf("`%s`\n", string);
        printf("`%zu`\n", string_size);
}
