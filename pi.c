/* Michael Ballantyne Homework 3 CMPT 351 Spring 2012 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long incircle = 0;
long points_per_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *runner() {
    long incircle_thread = 0;

    unsigned int rand_state = rand();
    long i;
    for (i = 0; i < points_per_thread; i++) {
        /* Was initially using random(), but didn't appear to get performance
         * improvement with threading. random() apparently uses some global state
         * that is shared between threads, and isn't guaranteed to be threadsafe. */
        double x = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;
        double y = rand_r(&rand_state) / ((double)RAND_MAX + 1) * 2.0 - 1.0;

        if (x * x + y * y < 1) {
            incircle_thread++;
        }
    }
    
    pthread_mutex_lock(&mutex);
    incircle += incircle_thread;
    pthread_mutex_unlock(&mutex);
}

/* Calculate Pi by the Monte Carlo method. Program arguments are the total number of random
 * points to use in the calculation and the number of threads to use. */
int main(int argc, const char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: ./pi <total points> <threads>\n");
        exit(1);
    }

    long totalpoints = atol(argv[1]);
    int thread_count = atoi(argv[2]);
    points_per_thread = totalpoints / thread_count;

    /* Tried using clock, but it measures CPU time, not wall clock time,
     * so doesn't demonstrate improvement gained by threading */
    time_t start = time(NULL);

    srand((unsigned)time(NULL));

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int i;
    for (i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], &attr, runner, (void *) NULL);
    }

    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);

    printf("Pi: %f\n", (4. * (double)incircle) / ((double)points_per_thread * thread_count));
    printf("Time: %d sec\n", (unsigned int)(time(NULL) - start));

    return 0;
}

