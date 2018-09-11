#pragma once
#include <stdint.h>

typedef enum {
  TokenTypeInvalid = 0,
  TokenTypeNumber = 1,
  TokenTypeLeftParens = 2,
  TokenTypeRightParens = 3,
  TokenTypeLeftBrace = 4,
  TokenTypeRightBrace = 5,
  TokenTypeComma = 6,
  TokenTypeDot = 7,
  TokenTypeMinus = 8,
  TokenTypePlus = 9,
  TokenTypeSemicolon = 10,
  TokenTypeStar = 11,
  TokenTypeBang = 12,
  TokenTypeBangEqual = 13,
  TokenTypeEqual = 14,
  TokenTypeEqualEqual = 15,
  TokenTypeLess = 16,
  TokenTypeLessEqual = 17,
  TokenTypeGreater = 18,
  TokenTypeGreaterEqual = 19,
  TokenTypeSlash = 20,
  TokenTypeString = 21,
  TokenTypeIdentifier = 22,
} TokenType;

typedef union {
  double number;
  const char *string;
} TokenValue;

typedef struct {
  TokenType type;
  TokenValue value;
} Token;

void tokenize(const char *characters, Token **tokens, uint64_t *tokens_count);
