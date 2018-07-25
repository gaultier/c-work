#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* line = NULL;
    size_t line_cap = 0;
    ssize_t line_length;

    const uint8_t people_size = 100;
    char* people[people_size];
    uint8_t people_count = 0;

    while ((line_length = getline(&line, &line_cap, stdin)) > 0) {
        if (!memcmp(line, "Revenue", sizeof("Revenue") - 1)) continue;
        if (!memcmp(line, "Expenses", sizeof("Expenses") - 1)) continue;
        if (line_length == 1 && line[0] == '\n') continue;

        char* token;
        while ((token = strsep(&line, " ")) != NULL) {
            if (*token == '\0') continue;
            uint64_t token_size = strlen(token);
            while (isspace(token[token_size - 1])) {
                token[token_size - 1] = '\0';
                token_size--;
            }

            printf("|%s|\n", token);
            people[people_count++] = strdup(token);
        }
    }

    for (uint8_t i = 0; i < people_count; i++) printf("%s\n", people[i]);
}
