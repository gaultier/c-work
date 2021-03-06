#include <curl/curl.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

void record_to_before_next(char** current, char needle, uint64_t size,
                           char* result);
void skip_to_after_next(char** current, char needle, uint64_t size);
void skip_to_next(char** current, char needle, uint64_t size);
double dist(double x1, double y1, double z1, double x2, double y2, double z2);
size_t curl_cb(void* content, size_t size, size_t nmemb, void* userp);

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

double dist(double x1, double y1, double z1, double x2, double y2, double z2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) +
           (z1 - z2) * (z1 - z2);
}

typedef struct {
    char* data;
    uint64_t size;
} Json;

size_t curl_cb(void* content, size_t size, size_t nmemb, void* userp) {
    uint64_t real_size = size * nmemb;
    Json* json = (Json*)userp;
    json->data = realloc(json->data, json->size + real_size + 1);
    if (!json->data) {
        printf("Out of memory\n");
        exit(1);
    }
    memcpy(&(json->data[json->size]), content, real_size);

    json->size += real_size;
    json->data[json->size] = 0;

    return real_size;
}

int main(int argc, const char* argv[]) {
    if (argc != 4) return 0;
    const uint64_t earth_radius = 6371;
    const double argv_latitude = M_PI * strtod(argv[1], NULL) / 180;
    const double argv_longitude = M_PI * strtod(argv[2], NULL) / 180;
    double argv_x = earth_radius * sin(argv_latitude) * cos(argv_longitude);
    double argv_y = earth_radius * sin(argv_latitude) * sin(argv_longitude);
    double argv_z = earth_radius * cos(argv_latitude);

    CURL* curl = curl_easy_init();
    Json json = {.size = 0, .data = malloc(1)};

    if (curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, argv[3]);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return 1;
        }
        curl_easy_cleanup(curl);
    }

    char* end = json.data + json.size;
    char* current = json.data;

    char best_ico[7] = "";
    char best_callsign[9] = "";
    char best_country[100] = "";
    double min_dist = DBL_MAX;

    while (current < end) {
        skip_to_next(&current, '[', (uint64_t)(end - current));
        if (!current) break;

        skip_to_after_next(&current, '"', (uint64_t)(end - current));
        char ico[7] = "";
        record_to_before_next(&current, '"', (uint64_t)(end - current), ico);
        skip_to_after_next(&current, '"', (uint64_t)(end - current));
        skip_to_after_next(&current, '"', (uint64_t)(end - current));

        char callsign[9] = "";
        record_to_before_next(&current, '"', (uint64_t)(end - current),
                              callsign);

        skip_to_after_next(&current, '"', (uint64_t)(end - current));
        skip_to_after_next(&current, '"', (uint64_t)(end - current));
        char country[100] = "";
        record_to_before_next(&current, '"', (uint64_t)(end - current),
                              country);

        skip_to_after_next(&current, ',', (uint64_t)(end - current));
        skip_to_after_next(&current, ',', (uint64_t)(end - current));
        skip_to_after_next(&current, ',', (uint64_t)(end - current));

        char longitude_str[100] = "";
        record_to_before_next(&current, ',', (uint64_t)(end - current),
                              longitude_str);
        double longitude_deg = strtod(longitude_str, NULL);
        skip_to_after_next(&current, ',', (uint64_t)(end - current));

        char latitude_str[100] = "";
        record_to_before_next(&current, ',', (uint64_t)(end - current),
                              latitude_str);
        double latitude_deg = strtod(latitude_str, NULL);
        skip_to_after_next(&current, ',', (uint64_t)(end - current));

        char altitude_str[100] = "";
        record_to_before_next(&current, ',', (uint64_t)(end - current),
                              altitude_str);
        double altitude = strtod(altitude_str, NULL);

        const double latitude_rad = M_PI * latitude_deg / 180;
        const double longitude_rad = M_PI * longitude_deg / 180;
        const double x =
            (earth_radius + altitude) * sin(latitude_rad) * cos(longitude_rad);
        const double y =
            (earth_radius + altitude) * sin(latitude_rad) * sin(longitude_rad);
        const double z = (earth_radius + altitude) * cos(latitude_rad);

        const double d = dist(argv_x, argv_y, argv_z, x, y, z);
        //               printf("X: %f Y: %f Z: %f D: %f\n", x, y, z, d);

        if (d < min_dist) {
            min_dist = d;
            strcpy(best_ico, ico);
            strcpy(best_callsign, callsign);
            strcpy(best_country, country);
        }
    }

    printf("Best distance: %.3f km\nIco: %s\nCall sign: %s\nCountry: %s\n",
           sqrt(min_dist), best_ico, best_callsign, best_country);

    return 0;
}
