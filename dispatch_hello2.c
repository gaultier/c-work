#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>

/*
 * An example of executing a set of blocks on a serial dispatch queue.
 * Usage: hello [name]...
 */
int
main(int argc, char *argv[])
{
    int i;

    /* Create a serial queue. */
    dispatch_queue_t greeter = dispatch_queue_create("Greeter", NULL);

    for (i = 1; i < argc; i++)
    {
        /* Add some work to the queue. */
        dispatch_async(greeter, ^{ printf("Hello %s!\n", argv[i]); });
    }

    /* Add a last item to the queue. */
    dispatch_sync(greeter, ^{ printf("Goodbye!\n"); });

    return 0;
}
