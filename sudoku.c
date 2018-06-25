#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_row_valid(const uint8_t board[][9], uint8_t row);
bool is_row_valid(const uint8_t board[][9], uint8_t row) {
    for (uint8_t i = 1; i <= 9; i++) {
        if (!memchr(&board[row], i, 9)) return false;
    }
    return true;
}

bool is_column_valid(const uint8_t board[][9], uint8_t column);
bool is_column_valid(const uint8_t board[][9], uint8_t column) {
    bool found = false;
    for (uint8_t i = 1; i <= 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            if (board[j][column] == i) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

bool is_square_valid(const uint8_t board[][9], uint8_t square);
bool is_square_valid(const uint8_t board[][9], uint8_t square) {
    bool found = false;
    for (uint8_t i = 1; i <= 9; i++) {
        for (uint8_t j = 0; j < 9; j++) {
            if (board[j][square] == i) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
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
        {8, 2, 4, 5, 6, 9, 8, 3, 7},
        {7, 3, 4, 5, 6, 9, 8, 3, 7},
        {6, 2, 4, 5, 6, 9, 8, 3, 7},
    };
    // clang-format on
    printf("%d\n", is_row_valid(board, 0));
    printf("%d\n", is_row_valid(board, 1));
    printf("%d\n", is_column_valid(board, 0));
    printf("%d\n", is_column_valid(board, 1));
}
