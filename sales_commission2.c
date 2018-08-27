#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint64_t line_number;
    char* str;
} Token;

uint64_t tokens_from_file(FILE* input, Token* tokens, uint64_t* token_count);
char* get_cell_str(Token const* cells, uint64_t people_count, uint64_t person_i,
                   uint64_t article_i);

uint64_t tokens_from_file(FILE* input, Token* tokens, uint64_t* token_count) {
    uint8_t line_number = 0;

    size_t line_cap = 0;
    ssize_t line_length;
    char* line = NULL;
    while ((line_length = getline(&line, &line_cap, input)) > 0) {
        if (line_length == 1 && line[0] == '\n') continue;

        char* token_str;
        while ((token_str = strsep(&line, " \n")) != NULL) {
            if (token_str[0] == '\0') continue;

            Token* token = &tokens[*token_count];
            token->line_number = line_number;
            token->str = strdup(token_str);
            *token_count += 1;
        }
        line_number++;
    }
    return line_number;
}

char* get_cell_str(Token const* cells, uint64_t people_count, uint64_t person_i,
                   uint64_t article_i) {
    uint64_t row_length = people_count + 1;
    return cells[row_length * article_i + person_i + 1].str;
}

int main() {
    Token* tokens = malloc(1000 * sizeof(Token));
    uint64_t token_count = 0;
    assert(tokens_from_file(stdin, tokens, &token_count) > 0);

    Token* person_token = &tokens[1];
    while (person_token->line_number == 1) person_token++;
    const uint64_t people_count = (uint64_t)(person_token - tokens - 1);

    Token* cell_revenue_token = person_token;
    while (strcmp(cell_revenue_token->str, "Expenses") != 0)
        cell_revenue_token++;
    const uint64_t cell_count = (uint64_t)(cell_revenue_token - person_token);
    const uint64_t article_count = cell_count / (people_count + 1);

    Token* const revenues = &tokens[1 + people_count];
    Token* const expenses = &tokens[1 + token_count / 2 + people_count];

    for (uint64_t p = 0; p < people_count; p++) {
        double commission_total = 0;
        for (uint64_t a = 0; a < article_count; a++) {
            char* revenue_str = get_cell_str(revenues, people_count, p, a);
            int64_t revenue = strtoll(revenue_str, NULL, 10);
            char* expense_str = get_cell_str(expenses, people_count, p, a);
            int64_t expense = strtoll(expense_str, NULL, 10);
            int64_t benefit = revenue - expense;
            double commission = benefit * 0.062;
            if (commission > 0) commission_total += commission;
        }
        printf("#Person %s: %.2f\n", tokens[1 + p].str, commission_total);
    }
}
