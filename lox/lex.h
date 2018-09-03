#pragma once
#include <stdint.h>

typedef enum {
  TokenTypeInvalid,
  TokenTypeNumber,
} TokenType;

typedef union {
  double number;
} TokenValue;

typedef struct {
  TokenType type;
  TokenValue value;
} Token;

void tokenize(const char *characters, Token **tokens, uint64_t *tokens_count);
