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

void tokenize(const char* characters, Token** tokens, uint64_t* tokens_count) {
    const char* current = characters;
    while (*current != '\0') {
        printf("[L000] %p `%c`\n", (const void*)current, *current);
        Token token = {.type = TokenTypeInvalid};

        if (char_is_digit(*current)) {
            parse_digits(&current, &token);
            vec_add(*tokens, *tokens_count, token);
        } else {
            switch (*current) {
                case '(':
                    token.type = TokenTypeLeftParens;
                    break;
                case ')':
                    token.type = TokenTypeRightParens;
                    break;
                case '{':
                    token.type = TokenTypeLeftBrace;
                    break;
                case '}':
                    token.type = TokenTypeRightBrace;
                    break;
                case ',':
                    token.type = TokenTypeComma;
                    break;
                case '.':
                    token.type = TokenTypeDot;
                    break;
                case '-':
                    token.type = TokenTypeMinus;
                    break;
                case '+':
                    token.type = TokenTypePlus;
                    break;
                case ';':
                    token.type = TokenTypeSemicolon;
                    break;
                case '*':
                    token.type = TokenTypeStar;
                    break;
            }
            vec_add(*tokens, *tokens_count, token);
            current += 1;
        }
    }
}
