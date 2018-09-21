#include "lex.h"
#include "parse.h"
#include "stdio.h"
#include "stdlib.h"

int main() {
    //    uint64_t tokens_count = 0;
    //    Token* tokens = NULL;
    //    tokenize("a b_c or and AND £ 12.54 \"yes", &tokens, &tokens_count);
    //
    //    for (uint64_t i = 0; i < tokens_count; i++) {
    //        switch (tokens[i].type) {
    //            case TokenTypeNumber:
    //                printf("[N001] %d %f\n", tokens[i].type,
    //                       tokens[i].value.number);
    //                break;
    //            case TokenTypeString:
    //                printf("[N002] %d `%s`\n", tokens[i].type,
    //                       tokens[i].value.string);
    //                break;
    //            case TokenTypeIdentifier:
    //                printf("[N003] %d `%s`\n", tokens[i].type,
    //                       tokens[i].value.string);
    //                break;
    //
    //            default:
    //                printf("[N004] %d\n", tokens[i].type);
    //        }
    //    }
    parse(NULL, 0, NULL, 0);
}
