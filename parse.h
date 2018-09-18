#pragma once

#include "lex.h"

typedef enum {
  ExprTypeInvalid,
  ExprTypePrimary,
  ExprTypeUnary,
  ExprTypeLiteral,
  ExprTypeGrouping,
} ExprType;

typedef struct {
  ExprType type;
  struct Expr *left;
  struct Expr *right;
} Expr;

void parse(const Token *tokens, uint64_t tokens_count, Expr **exprs,
           uint64_t exprs_count);
