#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "jsmn.h"
#include "math.h"
#include "utils.h"

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

    for (int i = 1; i < r; i++) {
        jsmntok_t token = tok[i];
        // printf("[%i]: type=%u, start=%u, end=%u, size=%u\n", i, token.type,
        //      token.start, token.end, token.size);

        char buf[100] = "";
        char* start = &content[token.start];
        uint64_t token_length = (uint64_t)(token.end - token.start + 1);
        strlcpy(buf, start, MIN(token_length, 100));

        if (token.type == JSMN_PRIMITIVE) {
            if (buf[0] == 'f')
                printf("\tfalse\n");
            else if (buf[0] == 't')
                printf("\ttrue\n");
            else if (buf[0] == 'n')
                printf("\tnull\n");
            else {
                double primitive = strtod(buf, NULL);
                if (primitive == HUGE_VAL || primitive == -HUGE_VAL)
                    fprintf(stderr,
                            "Could not convert primitive to double, would "
                            "overflow: %s errno=%d "
                            "(input was `%s`)\n",
                            strerror(errno), errno, buf);
                else
                    printf("\t%f\n", primitive);
            }
        } else if (token.type == JSMN_STRING)
            printf("\t'%s'\n", buf);
    }
    return 0;
}
