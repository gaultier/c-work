#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* typedef struct { */
/*     char* name; */
/*     int64_t amount; */
/* } Item; */

/* typedef enum { S_PEOPLE, S_REVENUE, S_EXPENSES } Section; */

int main() {
    char* line = NULL;
    size_t line_cap = 0;
    ssize_t line_length;
    const uint8_t max_size = 100;
    char* tokens[max_size][max_size];
    uint8_t line_count = 0;

    /* char* people[max_size]; */
    /* uint8_t people_count = 0; */
    /* Item revenue[max_size]; */
    /* uint8_t revenue_count = 0; */
    /* Item expenses[max_size]; */
    /* uint8_t expenses_count = 0; */

    /* Section section = S_PEOPLE; */

    /* if (section == S_PEOPLE) */
    /*           people[people_count++] = strdup(token); */
    /*       else if (section == S_REVENUE) { */
    /*           if (token_count == 0) */
    /*               first_token = strdup(token); */
    /*           else */
    /*               revenue[revenue_count++] = */
    /*                   (Item){.name = first_token, */
    /*                          .amount = strtoll(token, NULL, 10)}; */

    /*       } else { */
    /*           if (token_count == 0) */
    /*               first_token = strdup(token); */
    /*           else */
    /*               expenses[expenses_count++] = */
    /*                   (Item){.name = first_token, */
    /*                          .amount = strtoll(token, NULL, 10)}; */
    /*       } */

    while ((line_length = getline(&line, &line_cap, stdin)) > 0) {
        if (!memcmp(line, "Revenue", sizeof("Revenue") - 1)) continue;

        if (!memcmp(line, "Expenses", sizeof("Expenses") - 1)) continue;

        if (line_length == 1 && line[0] == '\n') continue;

        char* token;
        uint8_t token_count = 0;
        while ((token = strsep(&line, " ")) != NULL) {
            if (*token == '\0') continue;
            uint64_t token_size = strlen(token);
            while (isspace(token[token_size - 1])) {
                token[token_size - 1] = '\0';
                token_size--;
            }

            printf("|%s|\n", token);
            tokens[line_count][token_count++] = strdup(token);
        }
        tokens[line_count][token_count++] = NULL;
        line_count++;
    }

    char* token;
    uint8_t token_count = 0;
    for (uint8_t l = 0; l < line_count; l++) {
        while ((token = tokens[l][token_count++]) != NULL)
            printf("[%s]", token);
        printf("\n");
        token_count = 0;
    }
}
