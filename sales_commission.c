#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const uint8_t max_size = 100;
    char* tokens[max_size];
    uint8_t line_count = 0;
    uint8_t token_counts[max_size];
    uint8_t token_count = 0;

    char* line = NULL;
    size_t line_cap = 0;
    ssize_t line_length;
    while ((line_length = getline(&line, &line_cap, stdin)) > 0) {
        if (!memcmp(line, "Revenue", sizeof("Revenue") - 1)) continue;
        if (!memcmp(line, "Expenses", sizeof("Expenses") - 1)) continue;
        if (line_length == 1 && line[0] == '\n') continue;

        char* token;
        uint8_t line_token_count = 0;
        while ((token = strsep(&line, " ")) != NULL) {
            if (*token == '\0') continue;
            uint64_t token_size = strlen(token);
            while (isspace(token[token_size - 1])) {
                token[token_size - 1] = '\0';
                token_size--;
            }

            tokens[token_count] = strdup(token);
            line_token_count++;
            token_count++;
        }
        token_counts[line_count] = line_token_count;
        line_count++;
    }

    uint8_t people_count = token_counts[0];
    char** people = &tokens[0];
    uint8_t article_count = (line_count - 2) / 2;
    char** revenues = &tokens[people_count];
    char** expenses =
        &tokens[people_count + (1 + people_count) * article_count +
                people_count];

    for (uint8_t p = 0; p < people_count; p++) {
        double commission = 0;
        for (uint8_t a = 0; a < article_count; a++) {
            int64_t revenue =
                strtoll(revenues[(1 + people_count) * a + 1 + p], NULL, 10);
            int64_t expense =
                strtoll(expenses[(1 + people_count) * a + 1 + p], NULL, 10);
            int64_t benefit = revenue - expense;
            double com = benefit * 0.062;
            if (com > 0) commission += com;
        }
        printf("#Person %s: %.2f\n", people[p], commission);
    }
}
