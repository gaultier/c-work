#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "utils.h"

typedef struct BNode BNode;
struct BNode {
    uint32_t left_i;
    uint32_t right_i;
    uint8_t size;
    const char* value;
};

typedef struct {
    uint32_t* lefts_i;
    uint32_t* rights_i;
    uint8_t* sizes;
    const char** values;
    uint32_t count;
} BNodes;

typedef struct {
    const uint32_t str_i : 24, size : 8;
} String;

typedef struct {
    uint32_t count;
    String strings[];
} Strings;

int8_t cmp(const char s1[], uint8_t size1, const char s2[], uint8_t size2);
void bnode_insert(uint32_t parent_i, const char value[], uint8_t size,
                  uint32_t new_node_i, BNode nodes[]);
uint32_t bnode_find(uint32_t parent_i, const char value[], uint8_t size,
                    const BNode nodes[]);
const char* word_find_naive(const String words[], uint32_t words_size,
                            const char* word, uint8_t size,
                            const char* content);
const char* word_find_divide(const String words[], uint32_t words_size,
                             const char* word, uint8_t word_size,
                             const char* content);
const char* word_find_memcmp(char* content, uint64_t count, const char* word,
                             uint8_t word_size);

inline int8_t cmp(const char s1[], uint8_t size1, const char s2[],
                  uint8_t size2) {
    const uint8_t size = size1 < size2 ? size1 : size2;
    const int8_t compare = (int8_t)memcmp(s1, s2, size);
    return compare == 0 ? size1 - size2 : compare;
}

void bnode_insert(uint32_t parent_i, const char value[], uint8_t size,
                  uint32_t new_node_i, BNode nodes[]) {
    BNode* parent = &nodes[parent_i];
    uint32_t* child_i = cmp(value, size, parent->value, parent->size) < 0
                            ? &parent->left_i
                            : &parent->right_i;

    if (!*child_i) {
        BNode* new_node = &nodes[new_node_i];
        new_node->value = value;
        new_node->size = size;
        *child_i = new_node_i;
    } else
        bnode_insert(*child_i, value, size, new_node_i, nodes);
}

uint32_t bnode_find(uint32_t parent_i, const char value[], uint8_t size,
                    const BNode nodes[]) {
    const BNode* parent = &nodes[parent_i];
    const int8_t compare = cmp(value, size, parent->value, parent->size);
    if (compare == 0) {
        return parent_i;
    }

    const uint32_t child_i = compare < 0 ? parent->left_i : parent->right_i;

    if (!child_i) {
        return UINT32_MAX;
    }
    return bnode_find(child_i, value, size, nodes);
}

const char* word_find_naive(const String words[], uint32_t words_size,
                            const char* word, uint8_t word_size,
                            const char* content) {
    for (uint32_t i = 0; i < words_size; i++) {
        const char* cur_word = &content[words[i].str_i];
        if (memcmp(cur_word, word, word_size) == 0) {
            return cur_word;
        }
    }

    return NULL;
}

const char* word_find_divide(const String words[], uint32_t words_size,
                             const char* word, uint8_t word_size,
                             const char* content) {
    if (words_size <= 3)
        return word_find_naive(words, words_size, word, word_size, content);
    const uint32_t middle = words_size / 2 - 1;
    const String* word_middle = &words[middle];
    const char* word_middle_str = &content[word_middle->str_i];
    const int8_t compare =
        cmp(word, word_size, word_middle_str, word_middle->size);
    if (compare == 0) return word_middle_str;

    return compare < 0
               ? word_find_divide(words, middle, word, word_size, content)
               : word_find_divide(word_middle + 1, words_size - middle, word,
                                  word_size, content);
}

const char* word_find_memcmp(char* content, uint64_t content_size,
                             const char* word, uint8_t word_size) {
    const char* const end = content + (content_size - word_size);
    for (char* s = content; s < end; s++) {
        if (memcmp(s, word, word_size) == 0) {
            return s;
        }
    }
    return NULL;
}

int main(int argc, const char* argv[]) {
    if (argc != 3) exit(1);
    const uint8_t arg_word_size = strnlen(argv[2], 100);

    char* content;
    uint64_t size;
    int res = readFile(argv[1], &content, &size);
    if (res == -1) {
        fprintf(stderr, "Error reading file %s: %s\n", argv[1],
                strerror(errno));
        exit(1);
    }

    /////////////////////////////////////
    struct timeval time01;
    gettimeofday(&time01, NULL);
    uint64_t start = 0, end = 0;
    uint32_t count = 0;
    // BNode * const nodes = calloc(300 * 1000, sizeof(BNode));
    uint32_t strings_size = sizeof(uint32_t) + 300 * 1000 * sizeof(String);
    Strings* strings = calloc(1, strings_size);
    String* words = strings->strings;

    while (end < size) {
        if (content[end] == '\n') {
            const char* word = &content[start];
            const uint8_t word_size = end - start;
            words[count] = (String){.str_i = start, .size = word_size};

            // if (count == 0) {
            //   nodes[0].value = word;
            //   nodes[0].size = word_size;
            // } else {
            //   bnode_insert(0, word, word_size, count, nodes);
            // }

            start = end + 1;
            count++;
        }
        end++;
    }

    const int fd_write = open("words.bin", O_WRONLY | O_CREAT);
    if (fd_write == -1) {
        fprintf(stderr, "Error opening file: %s", strerror(errno));
        return -1;
    }

    printf("[D008] %u %lu\n", strings_size,
           sizeof(uint32_t) + count * sizeof(String));
    strings_size = sizeof(uint32_t) + count * sizeof(String);
    printf("[D009] %u\n", strings_size);
    strings = realloc(strings, strings_size);
    strings->count = count;

    int res_write = write(fd_write, strings, strings_size);
    if (res_write == -1 || res_write != strings_size)
        fprintf(stderr, "[1] Error writing file: %s\n", strerror(errno));
    printf("[D010] %d\n", res_write);
    fchmod(fd_write, S_IRUSR | S_IRGRP | S_IROTH);

    struct timeval time02;
    gettimeofday(&time02, NULL);

    printf("[Parse] %ld ms\n",
           ((time02.tv_sec * 1000 * 1000 + time02.tv_usec) -
            (time01.tv_sec * 1000 * 1000 + time01.tv_usec)) /
               1000);

    /////////////////////////////////////
    struct timeval time11;
    gettimeofday(&time11, NULL);
    const char* found_naive =
        word_find_naive(words, count, argv[2], arg_word_size, content);
    struct timeval time12;
    gettimeofday(&time12, NULL);
    printf("[Find naive] %p: %ld us\n", found_naive,
           (time12.tv_sec * 1000 * 1000 + time12.tv_usec) -
               (time11.tv_sec * 1000 * 1000 + time11.tv_usec));
    /////////////////////////////////////
    struct timeval time21;
    gettimeofday(&time21, NULL);
    const char* found_memcmp =
        word_find_memcmp(content, size, argv[2], arg_word_size);
    struct timeval time22;
    gettimeofday(&time22, NULL);
    printf("[Find memcmp] %p: %ld us\n", found_memcmp,
           (time22.tv_sec * 1000 * 1000 + time22.tv_usec) -
               (time21.tv_sec * 1000 * 1000 + time21.tv_usec));

    /////////////////////////////////////
    // struct timeval time31;
    // gettimeofday(&time31, NULL);
    // uint32_t found_tree_i = bnode_find(0, argv[2], strlen(argv[2]), nodes);
    // struct timeval time32;
    // gettimeofday(&time32, NULL);

    // printf("[Find tree] %d: %ld us\n",found_tree_i != UINT32_MAX,
    // (time32.tv_sec * 1000 * 1000 + time32.tv_usec) - (time31.tv_sec * 1000 *
    // 1000 + time31.tv_usec));
    /////////////////////////////////////
    struct timeval time41;
    gettimeofday(&time41, NULL);
    const char* found_divide =
        word_find_divide(words, count, argv[2], arg_word_size, content);
    struct timeval time42;
    gettimeofday(&time42, NULL);
    printf("[Find divide] %p: %ld us\n", found_divide,
           (time42.tv_sec * 1000 * 1000 + time42.tv_usec) -
               (time41.tv_sec * 1000 * 1000 + time41.tv_usec));
    /////////////////////////////////////
}
