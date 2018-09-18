#pragma once

#include "lex.h"

typedef struct {
} Expr;

void parse(const Token *tokens, uint64_t tokens_count, Expr **exprs,
           uint64_t exprs_count);
