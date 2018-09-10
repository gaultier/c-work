#include "lex.h"
#include <inttypes.h>
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "vec.h"

static bool char_is_digit(char c) { return c >= '0' && c <= '9'; }
static void parse_digits(const char** current, Token* token) {
    const uint64_t characters_count = strspn(*current, "0123456789");
    char* to_parse = malloc(characters_count + 1);
    strncpy(to_parse, *current, characters_count);
    to_parse[characters_count] = '\0';

    token->type = TokenTypeNumber;
    token->value.number = strtoll(to_parse, NULL, 10);

    free(to_parse);

    *current += characters_count;
}

static void add_token(const char** current, Token* token, TokenType type) {
    token->type = type;
    *current += 1;
}

void tokenize(const char* characters, Token** tokens, uint64_t* tokens_count) {
    const char* current = characters;
    while (*current != '\0') {
        printf("[L000] %p `%c`\n", current, *current);
        Token token = {.type = TokenTypeInvalid};

        if (char_is_digit(*current)) {
            parse_digits(&current, &token);
            vec_add(*tokens, *tokens_count, token);
        } else {
            switch (*current) {
                case '(':
                    add_token(&current, &token, TokenTypeLeftParens);
                    break;
                default:
                    vec_add(*tokens, *tokens_count, token);
                    current += 1;
            }
        }
    }
}
