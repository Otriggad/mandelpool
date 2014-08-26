/**
 * @file poolvsthread1run.c
 * @author Sebastian Rautila
 * @date 26/5 2014
 * @brief Measures runtime for one run of the same caluclation with and without the threadpool.
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

int main(int argc, char *argv[])
{

    if ( argc != 3 ) {
        printf("usage: %s numtasks poolsize\n", argv[0]);
        exit(0);
    }

    int NUMTASKS = atoi(argv[1]); //1600 // 16000
    int POOLSIZE = atoi(argv[2]); // 300 //868

    struct timespec tspec;
    long long withpool, nopool, before;
    static long long waittime = 500;

    clock_gettime(CLOCK_REALTIME, &tspec);
    before = timespecToMs(tspec);
    struct threadpool * pool = threadpool_create(POOLSIZE);
    for (int i=0; i < NUMTASKS; ++i) {
        threadpool_enqueue(pool, busywait, &waittime);
    }
    threadpool_destroy(pool);
    clock_gettime(CLOCK_REALTIME, &tspec);
    withpool = timespecToMs(tspec) - before;

    clock_gettime(CLOCK_REALTIME, &tspec);
    before = timespecToMs(tspec);
    pthread_t threads[NUMTASKS];
    for (int i=0; i < NUMTASKS; ++i) {
        pthread_create(&threads[i], NULL, busypthread, &waittime);
    }
    for(int i=0; i < NUMTASKS; ++i) {
        if(pthread_join(threads[i], NULL) != 0) {
            exit(EXIT_FAILURE);
        }
    }
    clock_gettime(CLOCK_REALTIME, &tspec);
    nopool = timespecToMs(tspec) - before;


    printf("Pool runtime: %lld\n",withpool);
    printf("Naked runtime: %lld\n",nopool);

    return 0;
}
