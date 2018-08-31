#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define vec_add(v, size, elem)                       \
    do {                                             \
        (v) = realloc(v, (size + 1) * sizeof(elem)); \
        (v)[size] = (elem);                          \
        (size)++;                                    \
    } while (0)

uint64_t split(char const* input, char*** names);

uint64_t split(char const* input, char*** names) {
    char* token_str = NULL;
    char* input_copy = strdup(input);
    uint64_t size = 0;
    while ((token_str = strsep(&input_copy, " \n")) != NULL) {
        if (token_str[0] == '\0') continue;
        printf("# `%s`\n", token_str);
        vec_add(*names, size, strdup(token_str));
    }

    free(input_copy);
    return size;
}

int main() {
    size_t ignore = 0;
    ssize_t line_size = 0;
    char* line;

    line_size = getline(&line, &ignore, stdin);  // Header `Revenue`
    line_size = getline(&line, &ignore, stdin);  // Empty line
    line_size = getline(&line, &ignore, stdin);  // Header list of names
    /* input[input_size - 1] = '\0'; */

    char** names = NULL;
    uint64_t names_count = split(line, &names);
    printf("Names #: %llu\n", names_count);
    for (uint64_t i = 0; i < names_count; i++) printf("N: `%s`\n", names[i]);

    char** articles = NULL;
    uint64_t articles_count = 0;
    char** expenses = NULL;
    uint64_t expenses_count = 0;
    do {
        line_size = getline(&line, &ignore, stdin);

        char** words = NULL;
        uint64_t words_count = 0;
        words_count = split(line, &words);

        if (words_count <= 1) continue;

        vec_add(articles, articles_count, words[0]);
        for (uint64_t w_i = 1; w_i < words_count; w_i++)
            vec_add(expenses, expenses_count, words[w_i]);

    } while (memmem(line, (uint64_t)line_size, "Expenses",
                    sizeof("Expenses") - 1) == NULL);

    for (uint64_t a_i = 0; a_i < articles_count; a_i++)
        printf("A: `%s`\n", articles[a_i]);
    for (uint64_t e_i = 0; e_i < expenses_count; e_i++)
        printf("E: `%s`\n", expenses[e_i]);

    line_size = getline(&line, &ignore, stdin);  // Empty line
    line_size = getline(&line, &ignore, stdin);  // Header list of names

    char** revenues = NULL;
    uint64_t revenues_count = 0;
    do {
        line_size = getline(&line, &ignore, stdin);

        char** words = NULL;
        uint64_t words_count = 0;
        words_count = split(line, &words);

        if (words_count <= 1) continue;

        for (uint64_t w_i = 1; w_i < words_count; w_i++)
            vec_add(revenues, revenues_count, words[w_i]);

    } while (line_size > 0);
    for (uint64_t r_i = 0; r_i < revenues_count; r_i++)
        printf("R: `%s`\n", revenues[r_i]);

    /* for (uint64_t p = 0; p < people_count; p++) { */
    /*     double commission_total = 0; */
    /*     for (uint64_t a = 0; a < article_count; a++) { */
    /*         char* revenue_str = get_cell_str(revenues, people_count, p,
     * a);
     */
    /*         int64_t revenue = strtoll(revenue_str, NULL, 10); */
    /*         char* expense_str = get_cell_str(expenses, people_count, p,
     * a);
     */
    /*         int64_t expense = strtoll(expense_str, NULL, 10); */
    /*         int64_t benefit = revenue - expense; */
    /*         double commission = benefit * 0.062; */
    /*         if (commission > 0) commission_total += commission; */
    /*     } */
    /*     printf("#Person %s: %.2f\n", tokens[1 + p].str,
     * commission_total); */
    /* } */
}
