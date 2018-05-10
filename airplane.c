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

bool str_eq(const char* content, int start, int end, char* s);
bool str_numerical(const char* content, int start, int end);
void str_fprintf(FILE* fd, const char* content, int start, int end);
uint64_t str_to_uint64(const char* content, int start, int end);

bool str_eq(const char* content, int start, int end, char* s) {
    return memcmp(s, &content[start], (uint64_t)(end - start)) == 0;
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
    if (num == 0 || num == ULLONG_MAX) {
        fprintf(stderr,
                "Could not convert primitive to uint64_t: %s errno=%d "
                "(input was `%s`)\n",
                strerror(errno), errno, buf);
    }
    return num;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) return 0;
    char* content;
    uint64_t size;
    int r = readFile(argv[1], &content, &size);
    if (r == -1) return 1;

    printf("Size: %llu\n", size);

    jsmn_parser p;
    jsmn_init(&p);
    const uint32_t tok_count = 1000;
    jsmntok_t* tok = calloc(sizeof(jsmntok_t), tok_count);

    r = jsmn_parse(&p, content, size, tok, tok_count);
    if (r < 0) {
        fprintf(stderr, "Failed parsing: %d\n", r);
        return 1;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || tok[0].type != JSMN_OBJECT) {
        fprintf(stderr, "Object expected\n");
        return 1;
    }
    printf("Found %d tokens\n", r);

    if (tok[1].type != JSMN_STRING ||
        !str_eq(content, tok[1].start, tok[1].end, "timestamp")) {
        fprintf(stderr, "`timestamp` expected, found `");
        str_fprintf(stderr, content, tok[1].start, tok[1].end);
        fprintf(stderr, "`\n");

        return 1;
    }

    if (tok[2].type != JSMN_PRIMITIVE ||
        !str_numerical(content, tok[2].start, tok[2].end)) {
        fprintf(stderr, "unix timestamp expected, found `");
        str_fprintf(stderr, content, tok[2].start, tok[2].end);
        fprintf(stderr, "`\n");

        return 1;
    }
    const uint64_t timestamp = str_to_uint64(content, tok[2].start, tok[2].end);
    printf("Timestamp=%llu\n", timestamp);
    if (tok[3].type != JSMN_STRING ||
        !str_eq(content, tok[3].start, tok[3].end, "states")) {
        fprintf(stderr, "`states` expected, found `");
        str_fprintf(stderr, content, tok[3].start, tok[3].end);
        fprintf(stderr, "`\n");

        return 1;
    }

    // for (int i = 3; i < r; i++) {
    //    jsmntok_t token = tok[i];
    //    // printf("[%i]: type=%u, start=%u, end=%u, size=%u\n", i,
    //    token.type,
    //    //      token.start, token.end, token.size);

    //    char buf[100] = "";
    //    char* start = &content[token.start];
    //    uint64_t token_length = (token.end - token.start);
    //    strlcpy(buf, start, MIN(token_length, 100));

    //    if (token.type == JSMN_PRIMITIVE) {
    //        if (buf[0] == 'f')
    //            printf("\tfalse\n");
    //        else if (buf[0] == 't')
    //            printf("\ttrue\n");
    //        else if (buf[0] == 'n')
    //            printf("\tnull\n");
    //        else {
    //            double primitive = strtod(buf, NULL);
    //            if (primitive == HUGE_VAL || primitive == -HUGE_VAL)
    //                fprintf(stderr,
    //                        "Could not convert primitive to double, would
    //                        " "overflow: %s errno=%d "
    //                        "(input was `%s`)\n",
    //                        strerror(errno), errno, buf);
    //            else
    //                printf("\t%f\n", primitive);
    //        }
    //    } else if (token.type == JSMN_STRING)
    //        printf("\t'%s'\n", buf);
    //    else if (token.type == JSMN_ARRAY) {
    //        printf("array[%u]\n", token.size);
    //    }
    //}
    return 0;
}
