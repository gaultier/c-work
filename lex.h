#pragma once
#include <stdint.h>

typedef enum {
  TokenTypeInvalid,
  TokenTypeNumber,
  TokenTypeLeftParens,
  TokenTypeRightParens,
  TokenTypeLeftBrace,
  TokenTypeRightBrace,
  TokenTypeComma,
  TokenTypeDot,
  TokenTypeMinus,
  TokenTypePlus,
  TokenTypeSemicolon,
  TokenTypeStar,
  TokenTypeBang,
  TokenTypeBangEqual,
  TokenTypeEqual,
  TokenTypeEqualEqual,
  TokenTypeLess,
  TokenTypeLessEqual,
  TokenTypeGreater,
  TokenTypeGreaterEqual,
  TokenTypeSlash,
} TokenType;

typedef union {
  double number;
} TokenValue;

typedef struct {
  TokenValue value;
  TokenType type;
} Token;

void tokenize(const char *characters, Token **tokens, uint64_t *tokens_count);
