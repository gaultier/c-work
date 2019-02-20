#include <os/log.h>

int main(int argc, char* argv[]) {
    os_log(OS_LOG_DEFAULT, "%s: %s [%s]", argv[1], argv[2], "chocolate");
}
