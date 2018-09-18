#include "parse.h"
#include "stdbool.h"
#include "stdlib.h"

static bool isAtEnd(const Token* current, const Token* tokens,
                    uint64_t tokens_count) {
    return (uint64_t)(current - tokens) / sizeof(current) == tokens_count - 1;
}
static const Token* advance(const Token** current, const Token* tokens,
                            uint64_t tokens_count) {
    if (!isAtEnd(*current, tokens, tokens_count)) *current += 1;
    return *current - 1;
}

static bool check(const Token* current, TokenType type, const Token* tokens,
                  uint64_t tokens_count) {
    if (isAtEnd(current, tokens, tokens_count))
        return false;
    else
        return current->type == type;
}

static bool match2(TokenType type1, TokenType type2, const Token** current,
                   const Token* tokens, uint64_t tokens_count) {
    if (check(*current, type1, tokens, tokens_count) ||
        check(*current, type2, tokens, tokens_count)) {
        advance(current, tokens, tokens_count);
        return true;
    }
    return false;
}

// static Expr* equality() {
//    Expr* expr = malloc(sizeof(Expr));
//    return expr;
//}
// static Expr* equality() {
//    Expr* expr = malloc(sizeof(Expr));
//    return expr;
//}
static Expr* binary(Expr* left, Token* operator, Expr* right) {
    Expr* expr = malloc(sizeof(Expr));
    *expr = (Expr){.left = left, .op = operator, .right = right };

    return expr;
}
static Expr* comparison() {
    Expr* expr = malloc(sizeof(Expr));
    return expr;
}

static Expr* equality() {
    Expr* left = comparison();

    while (match2(TokenTypeBangEqual, TokenTypeEqualEqual)) {
        Token* op = previous();
        Expr* right = comparison();
        left = binary(left, op, right);
    }

    return left;
}

static Expr* expression() { return equality(); }

void parse(const Token* tokens, uint64_t tokens_count, Expr** exprs,
           uint64_t exprs_count) {}
