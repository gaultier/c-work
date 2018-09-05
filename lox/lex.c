#include "lox/lex.h"
#include "lox/vec.h"
#include "stdbool.h"
#include "stdio.h"

static bool char_is_digit(char c) { return c >= '0' && c <= '9'; }
static void parse_digits(const char** current, Token* token) {
    const char* last_digit = current;  // FIXME

    token->type = TokenTypeNumber;
    token->value.number = 1;
}

void tokenize(const char* characters, Token** tokens, uint64_t* tokens_count) {
    const char* current = characters;
    while (*current != '\0') {
        Token token = {.type = TokenTypeInvalid};

        if (char_is_digit(*current)) {
            parse_digits(&current, &token);
            vec_add(*tokens, *tokens_count, token);
        } else
            vec_add(*tokens, *tokens_count, token);

        current++;
    }
}
