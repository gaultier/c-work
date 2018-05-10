#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "jsmn.h"
#include "math.h"
#include "utils.h"

bool str_eq(const char* content, int start, int end, char* s, uint64_t s_len);
bool str_numerical(const char* content, int start, int end);
void str_fprintf(FILE* fd, const char* content, int start, int end);
uint64_t str_to_uint64(const char* content, int start, int end);
double str_to_double(const char* content, int start, int end);

bool str_eq(const char* content, int start, int end, char* s, uint64_t s_len) {
    return (uint64_t)(end - start) == s_len &&
           memcmp(s, &content[start], (uint64_t)(end - start)) == 0;
}

void str_fprintf(FILE* fd, const char* content, int start, int end) {
    for (int64_t i = start; i < end; i++) fprintf(fd, "%c", content[i]);
}

bool str_numerical(const char* content, int start, int end) {
    for (int64_t i = start; i < end; i++) {
        if (content[i] < '0' || content[i] > '9') {
            return false;
        }
    }
    return true;
}

uint64_t str_to_uint64(const char* content, int start, int end) {
    int token_length = (end - start);
    char buf[token_length];
    memcpy(buf, &content[start], token_length);
    buf[token_length] = '\0';
    uint64_t num = strtoull(buf, NULL, 10);
    if (num == 0 || num == ULLONG_MAX)
        fprintf(stderr,
                "Could not convert primitive to uint64_t: %s errno=%d "
                "(input was `%s`)\n",
                strerror(errno), errno, buf);

    return num;
}

double str_to_double(const char* content, int start, int end) {
    int token_length = (end - start);
    char buf[token_length];
    memcpy(buf, &content[start], token_length);
    buf[token_length] = '\0';
    double num = strtod(buf, NULL);
    if (num == HUGE_VAL || num == -HUGE_VAL)
        fprintf(stderr,
                "Could not convert primitive to double: %s errno=%d "
                "(input was `%s`)\n",
                strerror(errno), errno, buf);

    return num;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) return 0;
    char* content;
    uint64_t size;
    const int read_result = readFile(argv[1], &content, &size);
    if (read_result == -1) return 1;

    printf("Size: %llu\n", size);

    jsmn_parser p;
    jsmn_init(&p);
    // Simple heuristic: JSON contains fewer tokens than bytes
    const uint32_t tok_expect = (uint32_t)size;
    jsmntok_t* tok = calloc(sizeof(jsmntok_t), tok_expect);

    const int tok_count = jsmn_parse(&p, content, size, tok, tok_expect);
    if (tok_count < 0) {
        fprintf(stderr, "Failed parsing: ret=%d, msg=%s\n", tok_count,
                tok_count == JSMN_ERROR_INVAL
                    ? "JSMN_ERROR_INVAL"
                    : tok_count == JSMN_ERROR_NOMEM
                          ? "JSMN_ERROR_NOMEM"
                          : tok_count == JSMN_ERROR_PART ? "JSMN_ERROR_PART"
                                                         : "Unknown error");
        return 1;
    }

    if (tok_count < 1 || tok[0].type != JSMN_OBJECT) {
        fprintf(stderr, "[0] Object expected, got type=%d\n", tok[0].type);
        return 1;
    }
    if (tok[0].size == 0) {
        fprintf(stderr, "[0] Empty object, got size=%d\n", tok[0].size);
        return 1;
    }

    printf("Found %d tokens\n", tok_count);

    if (tok[1].type != JSMN_STRING ||
        !str_eq(content, tok[1].start, tok[1].end, "timestamp",
                strlen("timestamp"))) {
        fprintf(stderr, "[1] `timestamp` expected, found `");
        str_fprintf(stderr, content, tok[1].start, tok[1].end);
        fprintf(stderr, "`\n");
        return 1;
    }

    if (tok[2].type != JSMN_PRIMITIVE ||
        !str_numerical(content, tok[2].start, tok[2].end)) {
        fprintf(stderr, "[2] unix timestamp expected, found `");
        str_fprintf(stderr, content, tok[2].start, tok[2].end);
        fprintf(stderr, "`\n");
        return 1;
    }

    const uint64_t timestamp = str_to_uint64(content, tok[2].start, tok[2].end);
    printf("Timestamp=%llu\n", timestamp);

    if (tok[3].type != JSMN_STRING || !str_eq(content, tok[3].start, tok[3].end,
                                              "states", strlen("states"))) {
        fprintf(stderr, "[3] `states` expected, found `");
        str_fprintf(stderr, content, tok[3].start, tok[3].end);
        fprintf(stderr, "`\n");
        return 1;
    }

    if (tok[4].type != JSMN_ARRAY || tok[4].size == 0) {
        fprintf(stderr, "[4] non empty array expected, found type=%d size=%d\n",
                tok[4].type, tok[4].size);
        return 1;
    }
    printf("%d states\n", tok[4].size);

    for (int i = 5; i < tok_count; i += 18) {
        if (tok[i].type != JSMN_ARRAY) {
            fprintf(stderr, "[%d] Expected array, got type=%d\n", i,
                    tok[i].type);
            return 1;
        } else if (tok[i].size != 17) {
            fprintf(stderr, "[%d] Unexpected array size: %d != 17\n", i,
                    tok[i].size);
            return 1;
        }

        printf("[%d] State %d:\n", i, (i - 5) / 18 + 1);

        for (int j = i + 1; j <= i + 17; j++) {
            if (tok[j].type != JSMN_PRIMITIVE && tok[j].type != JSMN_STRING) {
                fprintf(stderr,
                        "[%d][%d] Expected primitive or string, got type=%d\n",
                        i, j, tok[j].type);
                return 1;
            }
            printf("\t- ");
            str_fprintf(stdout, content, tok[j].start, tok[j].end);
            printf("\n");
        }
        double latitude =
            str_to_double(content, tok[i + 6].start, tok[i + 6].end);
        double longitude =
            str_to_double(content, tok[i + 7].start, tok[i + 7].end);
        double altitude =
            str_to_double(content, tok[i + 8].start, tok[i + 8].end);
        printf("***lat=%f\n", latitude);
        printf("***long=%f\n", longitude);
        printf("***alt=%f\n", altitude);
        // double latitude = str_to_double(content,  tok[j].start, tok[j].end);
        // double latitude = str_to_double(content,  tok[j].start, tok[j].end);
    }
    //    if (token.type == JSMN_PRIMITIVE) {
    //        if (buf[0] == 'f')
    //            printf("\tfalse\n");
    //        else if (buf[0] == 't')
    //            printf("\ttrue\n");
    //        else if (buf[0] == 'n')
    //            printf("\tnull\n");
    //        else {
    //            else
    //                printf("\t%f\n", primitive);
    //        }
    return 0;
}
