#include "parse.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#define DEBUG(x, fmt) \
    printf("%s:%d:%s: %s=" fmt "\n", __FILE__, __LINE__, __FUNCTION__, #x, x)

static bool isAtEnd(const Token* current, const Token* tokens,
                    uint64_t tokens_count) {
    return ((uint64_t)(current - tokens) / sizeof(current)) ==
           (tokens_count - 1);
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

static bool match4(TokenType type1, TokenType type2, TokenType type3,
                   TokenType type4, const Token** current, const Token* tokens,
                   uint64_t tokens_count) {
    if (check(*current, type1, tokens, tokens_count) ||
        check(*current, type2, tokens, tokens_count) ||
        check(*current, type3, tokens, tokens_count) ||
        check(*current, type4, tokens, tokens_count)) {
        advance(current, tokens, tokens_count);
        return true;
    }
    return false;
}

static Expr* binary(Expr* left, const Token* operator, Expr* right) {
    Expr* expr = malloc(sizeof(Expr));
    *expr = (Expr){.left = left, .op = operator, .right = right };

    return expr;
}

static Expr* primary(const Token** current, const Token* tokens,
                     uint64_t tokens_count) {
    return NULL;  // TODO
}

static Expr* expr_unary(Expr* op, Expr* right, const Token** current,
                        const Token* tokens, uint64_t tokens_count) {
    return NULL;  // TODO
}

static Expr* unary(const Token** current, const Token* tokens,
                   uint64_t tokens_count) {
    if (match2(TokenTypeBang, TokenTypeMinus, current, tokens, tokens_count)) {
        const Token* op = *(current - 1);
        Expr* right = unary(current, tokens, tokens_count);
        return expr_unary(op, right, current, tokens, tokens_count);
    }
    return primary(current, tokens, tokens_count);
}

static Expr* multiplication(const Token** current, const Token* tokens,
                            uint64_t tokens_count) {
    Expr* expr = unary(current, tokens, tokens_count);

    while (
        match2(TokenTypeSlash, TokenTypeStar, current, tokens, tokens_count)) {
        const Token* op = *(current - 1);
        Expr* right = unary(current, tokens, tokens_count);
        expr = binary(expr, op, right);
    }

    return expr;
}

static Expr* addition(const Token** current, const Token* tokens,
                      uint64_t tokens_count) {
    Expr* expr = multiplication(current, tokens, tokens_count);

    while (match4(TokenTypeGreater, TokenTypeGreaterEqual, TokenTypeLess,
                  TokenTypeLessEqual, current, tokens, tokens_count)) {
        const Token* op = *(current - 1);
        Expr* right = multiplication(current, tokens, tokens_count);
        expr = binary(expr, op, right);
    }

    return expr;
}

static Expr* comparison(const Token** current, const Token* tokens,
                        uint64_t tokens_count) {
    Expr* expr = addition(current, tokens, tokens_count);

    while (match4(TokenTypeGreater, TokenTypeGreaterEqual, TokenTypeLess,
                  TokenTypeLessEqual, current, tokens, tokens_count)) {
        const Token* op = *(current - 1);
        Expr* right = addition(current, tokens, tokens_count);
        expr = binary(expr, op, right);
    }

    return expr;
}

static Expr* equality(const Token** current, const Token* tokens,
                      uint64_t tokens_count) {
    Expr* expr = comparison(current, tokens, tokens_count);

    while (match2(TokenTypeBangEqual, TokenTypeEqualEqual, current, tokens,
                  tokens_count)) {
        const Token* op = *(current - 1);
        Expr* right = comparison(current, tokens, tokens_count);
        expr = binary(expr, op, right);
    }

    return expr;
}

static Expr* expression(const Token** current, const Token* tokens,
                        uint64_t tokens_count) {
    return equality(current, tokens, tokens_count);
}

static void expr_print(const Expr* expr) {
    if (!expr) return;

    printf("(");
    if (expr->op) token_print(expr->op);
    if (expr->left) expr_print(expr->left);
    if (expr->right) expr_print(expr->right);
    printf(")");
}

void parse(const Token* tokens, uint64_t tokens_count, Expr** exprs,
           uint64_t exprs_count) {
    //  const Token* current = tokens;
    //  const Token* end = tokens + tokens_count * sizeof(Token);
    // while (current != end) {
    // }

    uint64_t t_count = 0;
    Token* ts = NULL;
    tokenize("a != 1", &ts, &t_count);
    DEBUG(t_count, "%llu");
    printf("Tokens: ");
    for (uint64_t t_i = 0; t_i < t_count; t_i++) token_print(&ts[t_i]);
    printf("\n");
    const Token* c = ts;
    Expr* expr = expression(&c, ts, t_count);
    DEBUG((void*)expr, "%p");
    DEBUG((void*)expr->left, "%p");
    DEBUG((void*)expr->right, "%p");
    DEBUG((void*)expr->op, "%p");
    token_print(expr->op);
    printf("\n");
    printf("Expr: ");
    expr_print(expr);
}
