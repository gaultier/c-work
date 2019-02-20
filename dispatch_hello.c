#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>

/*
 * An example of executing a set of blocks on the main dispatch queue.
 * Usage: hello name ...
 */
int
main(int argc, char *argv[])
{
    int i;

    /*
     * Get the main serial queue.
     * It doesn't start processing until we call dispatch_main()
     */
    dispatch_queue_t main_q = dispatch_get_main_queue();

    for (i = 1; i < argc; i++)
    {
        /* Add some work to the main queue. */
        dispatch_async(main_q, ^{ printf("Hello %s!\n", argv[i]); });
    }

    /* Add a last item to the main queue. */
    dispatch_async(main_q, ^{ printf("Goodbye!\n"); });

    /* Start the main queue */
    dispatch_main();

    /* NOTREACHED */
    return 0;
}