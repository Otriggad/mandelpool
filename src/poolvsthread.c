/**
 * @file poolvsthread.c
 * @author Sebastian Rautila
 * @date 26/5 2014
 * @brief Generates .csv-file of runtime for the same task with various amount of threads in thread pool and also runtime without pool.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../include/threadpool.h"
#include <time.h>

long long timespecToMs(struct timespec spec)
{
    return(spec.tv_sec*1000LL + spec.tv_nsec/1000000);
}

void busywait(void * msec)
{
    long long waitmsec = *((long long*) msec);
    long long msbefore;
    long long msafter;
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);
    long randomval = rand();

    clock_gettime(CLOCK_REALTIME, &spec);
    msbefore = timespecToMs(spec);

    while(1) {
        if (randomval < 2135116019) {
            randomval = randomval + 1;
        }

        clock_gettime(CLOCK_REALTIME, &spec);
        msafter = timespecToMs(spec);
        if((msafter-msbefore) > waitmsec) {
            break;
        }
    }
}
void * busypthread(void *msec)
{
    busywait(msec);
    pthread_exit(NULL);
}

// for neatness datapoints should divide numtasks (this ensures we measure with as many threads as tasks).
//eg 1501 tasks 100 datapoints.
void poolCSV(int numtasks)
{
    long long timeval;
    struct timespec tspec;
    char filename[50];
    sprintf(filename,"csv/runtime_%i_tasks.csv",numtasks);
    FILE * outfile = fopen(filename, "wb");

    // run time for the spinning function
    static long long waittime = 500;
    struct threadpool * pool;

    // now lets try with threads in the interval up to the amount of tasks
    for (int numthreads=32; (numthreads <= numtasks) && (numthreads <= 4096); numthreads += numthreads) {
        // measure run time
        clock_gettime(CLOCK_REALTIME, &tspec);
        timeval = timespecToMs(tspec);
        pool = threadpool_create(numthreads);
        for (int j=0; j < numtasks; ++j) {
            threadpool_enqueue(pool, busywait, &waittime);
        }
        threadpool_destroy(pool);
        clock_gettime(CLOCK_REALTIME, &tspec);
        timeval = timespecToMs(tspec) - timeval;

        // write to file
        fprintf(outfile,"%i, %lld\n",numthreads, timeval);
    }

    // finaly we want to measure the naked runtime
    clock_gettime(CLOCK_REALTIME, &tspec);
    timeval = timespecToMs(tspec);
    pthread_t threads[numtasks];
    for (int i=0; i < numtasks; ++i) {
        pthread_create(&threads[i], NULL, busypthread, &waittime);
    }
    for(int i=0; i < numtasks; ++i) {
        if(pthread_join(threads[i], NULL) != 0) {
            exit(EXIT_FAILURE);
        }
    }
    clock_gettime(CLOCK_REALTIME, &tspec);
    timeval = timespecToMs(tspec) - timeval;
    fprintf(outfile,"\nRuntime without pool, %lld\n",timeval);
    fclose(outfile);
    return;
}

int main(int argc, char *argv[])
{

    if ( argc != 2 ) {
        printf("usage: %s numtasks\n", argv[0]);
        exit(0);
    }
    poolCSV(atoi(argv[1]));
    exit(0);
}
