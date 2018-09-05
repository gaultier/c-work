#include "lox/lex.h"
#include "assert.h"
#include "stdlib.h"

int main() {
    const char characters[] = "1 2 3";
    Token* tokens = NULL;
    uint64_t tokens_count = 0;
    tokenize(characters, &tokens, &tokens_count);

    assert(tokens_count == sizeof(characters) / sizeof(char) - 1);

    assert(tokens[0].type == TokenTypeNumber);
    assert(tokens[0].value.number == 1.0);

    assert(tokens[1].type == TokenTypeInvalid);

    assert(tokens[2].type == TokenTypeNumber);
    assert(tokens[2].value.number == 1.0);

    assert(tokens[3].type == TokenTypeInvalid);

    assert(tokens[4].type == TokenTypeNumber);
    assert(tokens[4].value.number == 1.0);

    assert(tokens[5].type == TokenTypeInvalid);
}
