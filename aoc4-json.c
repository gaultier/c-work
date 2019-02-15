#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum event_type { UNKOWN = 0, START_SHIFT, FALL_ASLEEP, WAKE_UP, COUNT };

int main() {
    json_t* events = json_array();

    {
        char* line = NULL;
        size_t line_cap = 0;
        ssize_t line_length;
        while ((line_length = getline(&line, &line_cap, stdin)) > 0) {
            char* date_separator = strchr(line, ']');
            size_t date_field_len = (size_t)(date_separator - line) + 1;
            char* date_field = strndup(line, date_field_len);
            date_field[date_field_len - 1] = '\0';

            struct tm time = {0};
            strptime(date_field, "[%F %H:%M", &time);
            time.tm_year = 90;  // HACK

            time_t timestamp = timegm(&time);
            char* needle_start = NULL;
            char needle[] = "Guard #";
            char* rest = date_separator;
            uint64_t guard_id = 0;
            if ((needle_start = strstr(rest, needle)) != NULL) {
                char* id_start = needle_start + sizeof(needle) - 1;
                char* id_end = NULL;
                guard_id = strtoul(id_start, &id_end, 10);
            }

            enum event_type event_type = UNKOWN;
            if (strstr(line, "begins shift"))
                event_type = START_SHIFT;
            else if (strstr(line, "wakes up"))
                event_type = WAKE_UP;
            else if (strstr(line, "falls asleep"))
                event_type = FALL_ASLEEP;

            json_array_append_new(
                events, json_pack("{s:i, s:i, s:i}", "timestamp", timestamp,
                                  "guard_id", guard_id, "type", event_type));
        }
    }

    json_dumpfd(events, 1, JSON_INDENT(2));
}
