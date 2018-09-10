#include "lex.h"
#include "stdio.h"
#include "stdlib.h"

int main() {
    uint64_t tokens_count = 0;
    Token* tokens = NULL;
    tokenize("a 123 b (){},.-+;*!!= = == < <= > >=", &tokens, &tokens_count);

    for (uint64_t i = 0; i < tokens_count; i++) {
        printf("[T00%llu] %d %f\n", i, tokens[i].type, tokens[i].value.number);
    }
}
