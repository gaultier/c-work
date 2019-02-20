#include <dispatch/dispatch.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/event.h>

int main(int argc, char* argv[]) {
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening `%s`: %d %s\n", argv[1], errno,
                strerror(errno));
        return errno;
    }
    dispatch_io_t chan = dispatch_io_create(
        DISPATCH_IO_STREAM, fd, dispatch_get_main_queue(), ^(int err) {
          printf("Clean-up time!\n");
          close(fd);
        });

    dispatch_source_t input_src = dispatch_source_create(
        DISPATCH_SOURCE_TYPE_READ, fd, 0, dispatch_get_main_queue());
    dispatch_source_set_event_handler(input_src, ^{
      int data = dispatch_source_get_data(input_src);
      printf("Event: %d\n", data);

      dispatch_io_read(
          chan, 0, SIZE_MAX, dispatch_get_main_queue(),
          ^(bool done, dispatch_data_t data, int err) {
            if (err) fprintf(stderr, "Error: %d %s\n", err, strerror(errno));

            bool success = dispatch_data_apply(
                data, ^bool(dispatch_data_t _Nonnull region, size_t offset,
                            const void* _Nonnull buff, size_t size) {
                  for (size_t i = 0; i < size; i++) putchar(((char*)buff)[i]);
                  return true;
                });
            if (!success) {
                fprintf(stderr, "Failure dispatch_data_apply\n");
            }
          });
    });
    dispatch_source_set_cancel_handler(input_src, ^{
      printf("End, closing fd\n");
      close(fd);
    });

    dispatch_activate(input_src);
    dispatch_main();
}
