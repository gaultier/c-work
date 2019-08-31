// cc -std=c99 -O2 -Weverything -std=c99 % && ./a.out
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
        FILE* const f = fopen("/Users/pgaultier/Downloads/aoc5.txt", "r");
        fseek(f, 0, SEEK_END);
        size_t string_size = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        char* const string = calloc(string_size + 1, 1);

        fread(string, 1, string_size, f);
        fclose(f);

        string[string_size] = 0;

        size_t i = 0;
        while (i < string_size) {
                if (abs(string[i] - string[i + 1]) == 32) {
                        memmove(string + i, string + i + 2,
                                string_size - i - 2);
                        string_size -= 2;
                        i = i > 0 ? i - 1 : 0;
                } else
                        i++;
        }

        printf("`%zu`\n", string_size - 1);
}
