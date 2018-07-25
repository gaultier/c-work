#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_array_valid(const uint8_t array[9]);
bool is_array_valid(const uint8_t array[9]) {
    for (uint8_t i = 1; i <= 9; i++) {
        if (!memchr(array, i, 9)) return false;
    }
    return true;
}

bool is_row_valid(const uint8_t board[][9], uint8_t row);
bool is_row_valid(const uint8_t board[][9], uint8_t row) {
    return is_array_valid(board[row]);
}

bool is_column_valid(const uint8_t board[][9], uint8_t column);
bool is_column_valid(const uint8_t board[][9], uint8_t column) {
    uint8_t array[9];
    for (uint8_t j = 0; j < 9; j++) array[j] = board[j][column];

    return is_array_valid(array);
}

bool is_square_valid(const uint8_t board[][9], uint8_t square);
bool is_square_valid(const uint8_t board[][9], uint8_t square) {
    uint8_t array[9];
    for (uint8_t i = 0; i < 3; i++)
        for (uint8_t j = i; j < 3; j++) array[i * 3 + j] = board[i][j];

    return is_array_valid(array);
}

int main() {
    // clang-format off
    const uint8_t board[9][ 9] = {
        {1, 9, 4, 5, 6, 9, 8, 3, 7},
        {9, 8, 3, 5, 7, 2, 1, 6, 4},
        {2, 7, 4, 5, 6, 9, 8, 3, 7},
        {3, 6, 4, 5, 6, 9, 8, 3, 7},
        {5, 5, 4, 5, 6, 9, 8, 3, 7},
        {4, 4, 4, 5, 6, 9, 8, 3, 7},
        {8, 2, 4, 5, 6, 9, 1, 2, 3},
        {7, 3, 4, 5, 6, 9, 6, 8, 7},
        {6, 2, 4, 5, 6, 9, 5, 9, 4},
    };
    // clang-format on
    printf("%d\n", is_row_valid(board, 0));
    printf("%d\n", is_row_valid(board, 1));
    printf("%d\n", is_column_valid(board, 0));
    printf("%d\n", is_column_valid(board, 1));
}
