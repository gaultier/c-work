#include <khash.h>
#include <kvec.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum event_type { UNKOWN = 0, START_SHIFT, FALL_ASLEEP, WAKE_UP, COUNT };
static const char* event_type_str[COUNT] = {"UNKOWN", "START_SHIFT",
                                            "FALL_ASLEEP", "WAKE_UP"};

struct event {
    uint64_t guard_id;
    time_t time;
    enum event_type event_type;
};

static int event_cmp_timestamp(const void* a, const void* b) {
    const struct event* x = a;
    const struct event* y = b;
    if (x->time < y->time)
        return -1;
    else if (x->time > y->time)
        return 1;
    else
        return 0;
}

#define ARR_OPTIMUM(arr, len, res, res_i, cmp)                     \
    do {                                                           \
        for (size_t i = 0; i < (len); i++)                         \
            if (((arr)[i])cmp(res)) (res) = (arr)[i], (res_i) = i; \
    } while (0);

KHASH_MAP_INIT_INT(hash, uint64_t)

struct sleep_stat {
    uint64_t guard_id, minutes_asleep;
    time_t fall_asleep_timestamp, wake_up_timestamp;
    uint8_t fall_asleep_minute, wake_up_minute;
};

static int sleep_stat_cmp_guard_id(const void* a, const void* b) {
    const struct sleep_stat* x = a;
    const struct sleep_stat* y = b;
    if (x->guard_id < y->guard_id)
        return -1;
    else if (x->guard_id > y->guard_id)
        return 1;
    else
        return 0;
}

KHASH_MAP_INIT_INT(st, struct sleep_stat);

int main() {
    kvec_t(struct event) events;
    kv_init(events);

    {
        char* line = NULL;
        size_t line_cap = 0;
        ssize_t line_length;
        while ((line_length = getline(&line, &line_cap, stdin)) > 0) {
            char* date_separator = strchr(line, ']');
            size_t date_field_len = (size_t)(date_separator - line) + 1;
            char* date_field = strndup(line, date_field_len);
            date_field[date_field_len - 1] = '\0';

            struct event e = {0};

            struct tm time = {0};
            strptime(date_field, "[%F %H:%M", &time);
            time.tm_year = 90;  // HACK

            e.time = timegm(&time);

            char* needle_start = NULL;
            char needle[] = "Guard #";
            char* rest = date_separator;
            if ((needle_start = strstr(rest, needle)) != NULL) {
                char* id_start = needle_start + sizeof(needle) - 1;
                char* id_end = NULL;
                e.guard_id = strtoul(id_start, &id_end, 10);
            }

            if (strstr(line, "begins shift"))
                e.event_type = START_SHIFT;
            else if (strstr(line, "wakes up"))
                e.event_type = WAKE_UP;
            else if (strstr(line, "falls asleep"))
                e.event_type = FALL_ASLEEP;

            kv_push(struct event, events, e);
        }
    }

    {
        qsort(&kv_A(events, 0), kv_size(events), sizeof(struct event),
              event_cmp_timestamp);

        uint64_t current_guard_id = 0;
        for (size_t i = 0; i < kv_size(events); i++) {
            struct event* e = &kv_A(events, i);
            if (e->guard_id)
                current_guard_id = e->guard_id;
            else
                e->guard_id = current_guard_id;
        }
    }

    for (size_t i = 0; i < kv_size(events); i++) {
        struct event* e = &kv_A(events, i);
        printf("- Guard id: %llu\n  Time: %ld %s  Event type: %s\n",
               e->guard_id, e->time, ctime(&e->time),
               event_type_str[e->event_type]);
    }

    khash_t(hash)* minutes_asleep_per_guard = kh_init(hash);
    {
        khiter_t k;
        time_t fall_asleep_timestamp = 0;
        for (size_t i = 0; i < kv_size(events); i++) {
            struct event* e = &kv_A(events, i);

            if (e->event_type == FALL_ASLEEP) fall_asleep_timestamp = e->time;
            if (e->event_type == WAKE_UP) {
                uint64_t asleep_duration =
                    (uint64_t)lround(difftime(e->time, fall_asleep_timestamp)) /
                    60;
                fall_asleep_timestamp = 0;

                int ret;
                k = kh_put(hash, minutes_asleep_per_guard, e->guard_id, &ret);
                if (ret == 0)
                    kh_value(minutes_asleep_per_guard, k) += asleep_duration;
                else
                    kh_value(minutes_asleep_per_guard, k) = asleep_duration;
            }
        }
    }

    uint64_t max_guard_id = 0;
    uint64_t max_asleep = 0;
    {
        uint64_t guard_id = 0;
        uint64_t asleep_duration = 0;
        kh_foreach(minutes_asleep_per_guard, guard_id, asleep_duration, {
            if (max_asleep < asleep_duration) {
                max_asleep = asleep_duration;
                max_guard_id = guard_id;
            }
        });
    }
    printf("Longest asleep: Guard id=%llu, sleep duration=%llu m\n",
           max_guard_id, max_asleep);

    uint64_t sleep_minute_count[60];
    bzero(sleep_minute_count, sizeof(sleep_minute_count));
    {
        time_t fall_asleep_timestamp = 0;
        for (size_t i = 0; i < kv_size(events); i++) {
            struct event* e = &kv_A(events, i);
            if (e->guard_id == max_guard_id) {
                if (e->event_type == FALL_ASLEEP)
                    fall_asleep_timestamp = e->time;
                else if (e->event_type == WAKE_UP) {
                    struct tm wake_up_time = {0};
                    gmtime_r(&e->time, &wake_up_time);

                    struct tm fall_asleep_time = {0};
                    gmtime_r(&fall_asleep_timestamp, &fall_asleep_time);
                    int fall_asleep_minute = fall_asleep_time.tm_min;

                    uint64_t diff =
                        difftime(e->time, fall_asleep_timestamp) / 60;

                    for (uint64_t j = 0; j < diff; j++)
                        sleep_minute_count[(fall_asleep_minute + j) % 60] += 1;

                    fall_asleep_timestamp = 0;
                }
            }
        }
    }

    uint8_t min_minute = 0;
    uint8_t min_minute_count = 0;
    for (uint8_t i = 0; i < 60; i++) {
        printf("%hhu: %llu\n", i, sleep_minute_count[i]);
        if (min_minute_count < sleep_minute_count[i]) {
            min_minute_count = sleep_minute_count[i];
            min_minute = i;
        }
    }

    printf("Most frequent asleep: minute=%d, count=%hhu\n", min_minute,
           min_minute_count);

    {
        kvec_t(struct sleep_stat) sleep_stats = {0};

        time_t fall_asleep_timestamp = 0;
        for (size_t i = 0; i < kv_size(events); i++) {
            struct event* e = &kv_A(events, i);

            if (e->event_type == FALL_ASLEEP)
                fall_asleep_timestamp = e->time;
            else if (e->event_type == WAKE_UP) {
                uint64_t minutes_asleep =
                    difftime(e->time, fall_asleep_timestamp) / 60;

                struct sleep_stat s = {
                    .guard_id = e->guard_id,
                    .minutes_asleep = minutes_asleep,
                    .fall_asleep_timestamp = fall_asleep_timestamp,
                    .wake_up_timestamp = e->time,
                    .fall_asleep_minute =
                        gmtime(&fall_asleep_timestamp)->tm_min,
                    .wake_up_minute = gmtime(&e->time)->tm_min};

                kv_push(struct sleep_stat, sleep_stats, s);

                fall_asleep_timestamp = 0;
            }
        }

        kvec_t(uint64_t) guard_ids = {0};

        uint64_t guard_most_asleep_id = 0;
        uint64_t most_asleep_minutes = 0;
        printf("Longest asleep: Guard id=%llu, sleep duration=%llu m\n",
               guard_most_asleep_id, most_asleep_minutes);

        uint64_t sleep_minute_count[60];
        bzero(sleep_minute_count, sizeof(sleep_minute_count));

        for (uint64_t i = 0; i < kv_size(sleep_stats); i++) {
            struct sleep_stat* s = &kv_A(sleep_stats, i);

            for (uint64_t m = 0;
                 s->guard_id == guard_most_asleep_id && m < s->minutes_asleep;
                 m++)
                sleep_minute_count[(s->fall_asleep_minute + m) % 60] += 1;
        }

        for (uint8_t i = 0; i < 60; i++)
            printf("%hhu: %llu\n", i, sleep_minute_count[i]);

        uint8_t minute = 0;
        uint64_t count = 0;
        ARR_OPTIMUM(sleep_minute_count, 60, count, minute, >);
        printf("Minute: %hhu, count: %llu\n", minute, count);
    }
}
