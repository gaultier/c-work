#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* tmp = NULL;
static size_t tmp_size = 0;

static void memcpy_without_sub_in_place(char* src, size_t* src_size,
                                        size_t sub_i, size_t sub_size) {
        tmp_size = *src_size - sub_size;

        memcpy(tmp, src, sub_i);
        memcpy(tmp + sub_i, src + sub_i + sub_size,
               *src_size - (sub_i + sub_size));
        memset(tmp + tmp_size, 0, *src_size - tmp_size);

        memcpy(src, tmp, tmp_size);
        memset(src + tmp_size, 0, *src_size - tmp_size);
        *src_size = tmp_size;
}

int main() {
        FILE* const f = fopen("/Users/pgaultier/Downloads/aoc5.txt", "r");
        fseek(f, 0, SEEK_END);
        size_t string_size = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        char* const string = calloc(string_size + 1, 1);

        fread(string, 1, string_size, f);
        fclose(f);

        string[string_size] = 0;
        size_t i = string_size - 1;
        // 'Remove' trailing whitespace
        while (string[i] == ' ' || string[i] == '\n' || string[i] == '\t') {
                string[i] = 0;
                string_size--;
                i--;
        }
        tmp = malloc(string_size);
        tmp_size = string_size;
        memcpy(tmp, string, string_size);

        i = 0;
        while (i < string_size) {
                const char a = string[i];
                const char b = string[i + 1];
                if (abs(a - b) == 32) {
                        memcpy_without_sub_in_place(string, &string_size, i, 2);
                        i = 0;
                        continue;
                }
                i++;
        }

        printf("`%s`\n", string);
        printf("`%zu`\n", string_size);
}
