#pragma once

#include "lex.h"

typedef enum {
  ExprTypeInvalid,
  ExprTypePrimary,
  ExprTypeUnary,
  ExprTypeLiteral,
  ExprTypeGrouping,
} ExprType;

typedef struct Expr Expr;

struct Expr {
  ExprType type;
  Expr *left;
  const Token *op;
  Expr *right;
};

void parse(const Token *tokens, uint64_t tokens_count, Expr **exprs,
           uint64_t exprs_count);
