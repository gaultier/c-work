/*
 * usage: source_timer [duration count]...
 *
 * example: source_timer 1 10 2 7 5 5
 *
 *     Creates a 1 second timer that counts down 10 times (10 second total),
 *     a 2 second timer that counts down 7 times (14 second total),
 *     and a 5 second timer that counts down 5 times (25 second total).
 */
#include <stdio.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>

/*
 * We keep track of how many timers are active with ntimers.
 * The ntimer_serial queue is used to update the variable.
 */
static uint32_t ntimers = 0;
static dispatch_queue_t ntimer_serial = NULL;

/*
 * The print_serial queue is used to serialize printing.
 */
static dispatch_queue_t print_serial = NULL;

int
main(int argc, char *argv[])
{
    dispatch_queue_t main_q;
    int i;

    /*
     * Create our serial queues and get the main queue.
     */
    print_serial = dispatch_queue_create("Print Queue", NULL);
    ntimer_serial = dispatch_queue_create("N Timers Queue", NULL);

    main_q = dispatch_get_main_queue();

    /*
     * Pick up arguments two at a time.
     */
    for (i = 1; i < argc; i += 2)
    {
        uint64_t duration;
        dispatch_source_t src;

        /*
         * The count variable is declared with __block to make it read/write.
         */
        __block uint32_t count;

        /*
         * Timers are in nanoseconds.  NSEC_PER_SEC is defined by libdispatch.
         */
        duration = atoll(argv[i]) * NSEC_PER_SEC;
        count = atoi(argv[i + 1]);

        /*
         * Create a dispatch source for a timer, and pair it with the main queue.
         */
        src = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, main_q);

        /*
         * Set the timer's duration (in nanoseconds).
         */
        dispatch_source_set_timer(src, 0, duration, 0);

        /*
         * Set an event handler block for the timer source.
         * This is the block of code that will be executed when the timer fires.
         */
        dispatch_source_set_event_handler(src, ^{
            /* Count down to zero */
            count--;
            dispatch_async(print_serial, ^{ printf("%s second timer   count %u\n", argv[i], count); });

            if (count == 0) 
            {
                /*
                 * When the counter hits zero, we cancel and release the
                 * timer source, and decrement the count of active timers.
                 */
                dispatch_source_cancel(src);
                dispatch_release(src);
                dispatch_sync(ntimer_serial, ^{ ntimers--; });
            }

            if (ntimers == 0)
            {
                /*
                 * This was the last active timer.  Say goodbye and exit.
                 */
                dispatch_sync(print_serial, ^{ printf("All timers finished.  Goodbye\n"); });
                exit(0);
            }
        });

        /*
         * Increment the count of active timers, and activate the timer source.
         */
        dispatch_sync(ntimer_serial, ^{ ntimers++; });
        dispatch_resume(src);
    }

    /*
     * When we reach this point, all the timers have been created and are active.
     * Start the main queue to process the event handlers.
     */
    dispatch_main();

    return 0;
}
