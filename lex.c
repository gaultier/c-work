#include "lex.h"
#include <inttypes.h>
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "vec.h"

inline static bool char_is_digit(char c) { return c >= '0' && c <= '9'; }
static uint64_t number(const char* current, double* value) {
    const uint64_t characters_count = strspn(current, "0123456789.");
    char* to_parse = malloc(characters_count + 1);
    strncpy(to_parse, current, characters_count);
    to_parse[characters_count] = '\0';
    *value = strtod(to_parse, NULL);

    free(to_parse);

    return characters_count;
}

inline static char peekNext(const char* current) {
    return current[0] == '\0' ? '\0' : current[1];
}

static bool match(const char** current, char character) {
    if (**current != character) return false;

    *current += 1;
    return true;
}

static void add_token_with_value(const char** characters, Token** tokens,
                                 uint64_t* tokens_count, TokenType type,
                                 TokenValue value, uint64_t characters_count) {
    Token token = {.type = type, .value = value};
    vec_add(*tokens, *tokens_count, token);
    *characters += characters_count;
}

static void add_token(const char** characters, Token** tokens,
                      uint64_t* tokens_count, TokenType type) {
    const TokenValue value = {0};
    add_token_with_value(characters, tokens, tokens_count, type, value, 1);
}

static const char* string(const char* characters) {
    const char* end = characters + 1;
    while (*end != '\0' && *end != '"') end += 1;
    if (*end == '\0') exit(1);

    char* result = strndup(characters + 1, (size_t)(end - characters));
    result[end - characters - 1] = '\0';

    return result;
}

void tokenize(const char* characters, Token** tokens, uint64_t* tokens_count) {
    const char* current = characters;
    while (*current != '\0') {
        printf("[L000] `%c`\n", *current);

        if (char_is_digit(*current)) {
            TokenValue value = {0};
            const uint64_t characters_count = number(current, &value.number);
            add_token_with_value(&current, tokens, tokens_count,
                                 TokenTypeNumber, value, characters_count);
        } else {
            switch (*current) {
                case '(':
                    add_token(&current, tokens, tokens_count,
                              TokenTypeLeftParens);
                    break;
                case ')':
                    add_token(&current, tokens, tokens_count,
                              TokenTypeRightParens);
                    break;
                case '{':
                    add_token(&current, tokens, tokens_count,
                              TokenTypeLeftBrace);
                    break;
                case '}':
                    add_token(&current, tokens, tokens_count,
                              TokenTypeRightBrace);
                    break;
                case ',':
                    add_token(&current, tokens, tokens_count, TokenTypeComma);
                    break;
                case '.':
                    add_token(&current, tokens, tokens_count, TokenTypeDot);
                    break;
                case '-':
                    add_token(&current, tokens, tokens_count, TokenTypeMinus);
                    break;
                case '+':
                    add_token(&current, tokens, tokens_count, TokenTypePlus);
                    break;
                case ';':
                    add_token(&current, tokens, tokens_count,
                              TokenTypeSemicolon);
                    break;
                case '*':
                    add_token(&current, tokens, tokens_count, TokenTypeStar);
                    break;
                case '!':
                    add_token(&current, tokens, tokens_count,
                              match(&current, '=') ? TokenTypeBangEqual
                                                   : TokenTypeBang);
                    break;
                case '=':
                    add_token(&current, tokens, tokens_count,
                              match(&current, '=') ? TokenTypeEqualEqual
                                                   : TokenTypeEqual);
                    break;
                case '<':
                    add_token(&current, tokens, tokens_count,
                              match(&current, '=') ? TokenTypeLessEqual
                                                   : TokenTypeLess);
                    break;
                case '>':
                    add_token(&current, tokens, tokens_count,
                              match(&current, '=') ? TokenTypeGreaterEqual
                                                   : TokenTypeGreater);
                    break;
                case '/':
                    if (match(&current, '/')) {
                        while (*current != '\n' && *current != '\0')
                            current += 1;
                    } else {
                        add_token(&current, tokens, tokens_count,
                                  TokenTypeSlash);
                    }
                    break;
                case ' ':
                case '\r':
                case '\t':
                    current += 1;
                    break;
                case '\n':
                    // TODO: line++
                    current += 1;
                    break;
                case '"': {
                    TokenValue value = {.string = string(current)};
                    add_token_with_value(&current, tokens, tokens_count,
                                         TokenTypeString, value,
                                         strlen(value.string) + 2);
                } break;
                default:
                    add_token(&current, tokens, tokens_count, TokenTypeInvalid);
            }
        }
    }
}
