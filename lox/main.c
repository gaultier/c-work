#include "lox/lex.h"
#include "stdlib.h"

int main() {
    uint64_t tokens_count = 0;
    tokenize(NULL, NULL, &tokens_count);
}
