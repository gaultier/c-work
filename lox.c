#include "lex.h"
#include "stdio.h"
#include "stdlib.h"

int main() {
    uint64_t tokens_count = 0;
    Token* tokens = NULL;
    tokenize("ab 123.456!(){},.-+;*!!= = == < <= > >=// abc\n56/ \"hello\"+",
             &tokens, &tokens_count);

    for (uint64_t i = 0; i < tokens_count; i++) {
        if (tokens[i].type == TokenTypeNumber)
            printf("[N001] %d %f\n", tokens[i].type, tokens[i].value.number);
        else if (tokens[i].type == TokenTypeString)
            printf("[N002] %d `%s`\n", tokens[i].type, tokens[i].value.string);
        else
            printf("[N003] %d\n", tokens[i].type);
    }
}
