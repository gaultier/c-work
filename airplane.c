#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "math.h"
#include "utils.h"

void record_to_before_next(char** current, char needle, uint64_t size,
                           char* result);
void skip_to_after_next(char** current, char needle, uint64_t size);
void skip_to_next(char** current, char needle, uint64_t size);

void skip_to_next(char** current, char needle, uint64_t size) {
    *current = memchr(*current, needle, size);
}

void skip_to_after_next(char** current, char needle, uint64_t size) {
    skip_to_next(current, needle, size);
    (*current)++;
}

void record_to_before_next(char** current, char needle, uint64_t size,
                           char* result) {
    char* end = *current;
    skip_to_next(&end, needle, size);
    memcpy(result, *current, end - *current);
    *current = end;
}

int main(int argc, const char* argv[]) {
    if (argc != 4) return 0;
    const double argv_latitude = M_PI * strtod(argv[2], NULL) / 180;
    const double argv_longitude = M_PI * strtod(argv[3], NULL) / 180;
    const uint64_t earth_radius = 6371000;
    const double argv_thetha = argv_longitude;
    const double argv_phi = M_PI - argv_latitude;

    double argv_x = earth_radius * sin(argv_thetha) * cos(argv_phi);
    double argv_y = earth_radius * sin(argv_thetha) * sin(argv_phi);
    double argv_z = earth_radius * cos(argv_thetha);
    printf("Lat=%f Lng=%f X=%f Y=%f Z=%f\n", argv_latitude, argv_longitude,
           argv_x, argv_y, argv_z);

    char* content;
    uint64_t size;
    const int read_result = readFile(argv[1], &content, &size);
    if (read_result == -1) return 1;
    char* end = content + size;
    printf("Size: %llu\n", size);

    char* current = content;
    skip_to_next(&current, '[', size);
    skip_to_after_next(&current, '"', (uint64_t)(end - current));
    char ico[7] = "";
    record_to_before_next(&current, '"', (uint64_t)(end - current), ico);
    printf("***%s***\n", ico);
    skip_to_after_next(&current, '"', (uint64_t)(end - current));
    skip_to_after_next(&current, '"', (uint64_t)(end - current));

    char callsign[9] = "";
    record_to_before_next(&current, '"', (uint64_t)(end - current), callsign);
    printf("***%s***\n", callsign);

    skip_to_after_next(&current, '"', (uint64_t)(end - current));
    skip_to_after_next(&current, '"', (uint64_t)(end - current));
    char country[100] = "";
    record_to_before_next(&current, '"', (uint64_t)(end - current), country);
    printf("***%s***\n", country);

    skip_to_after_next(&current, ',', (uint64_t)(end - current));
    skip_to_after_next(&current, ',', (uint64_t)(end - current));
    skip_to_after_next(&current, ',', (uint64_t)(end - current));

    char longitude_str[100] = "";
    record_to_before_next(&current, '"', (uint64_t)(end - current),
                          longitude_str);
    double longitude = strtod(longitude_str, NULL);
    printf("Lng: %f\n", longitude);

    // current = memchr(current, ',', current - content) + 1;
    // current = memchr(current, ',', current - content) + 1;

    // char* longitude_end = memchr(current, ',', current - content) - 1;
    // char longitude_str[100] = "";
    // memcpy(longitude_str, current, longitude_(uint64_t)(end - current));
    // double longitude = strtod(longitude_str, NULL);
    // printf("Lng: %f\n", longitude);
    // current = longitude_end + 2;

    // char* latitude_end = memchr(current, ',', current - content) - 1;
    // char latitude_str[100] = "";
    // memcpy(latitude_str, current, latitude_(uint64_t)(end - current));
    // double latitude = strtod(latitude_str, NULL);
    // printf("Lat: %f\n", latitude);
    // current = latitude_end + 2;

    // char* altitude_end = memchr(current, ',', current - content) - 1;
    // char altitude_str[100] = "";
    // memcpy(altitude_str, current, altitude_(uint64_t)(end - current));
    // double altitude = strtod(altitude_str, NULL);
    // printf("Alt: %f\n", altitude);
    // current = altitude_end + 2;

    // printf("current: %c (%llu)\n", *current, current - content);

    // char* end = memchr(current, ']', size - current_i);
    // uint64_t end_i = end - content;
    // printf("End: %c (%llu)\n", *end, end_i);

    return 0;
}
