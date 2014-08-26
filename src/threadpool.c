/**
 * @file threadpool.c
 * @author Sebastian Rautila, Andreas Widmark
 * @date 7/5 2014
 * @brief Thread pool with constant amount of threads
 */

#include "../include/threadpool.h"

////////////
//Structs//
///////////

/**
 * @struct job
 * @brief @ref job containing a routine and its arguments
 *
 */
typedef struct job {
    /*@{*/
    void (*routine)(void*); /**< the function to be executed */
    void * arg; /**< the arguments for the routine */
    /*@}*/
} job;

/**
 * @struct jobQueue
 * @brief struct containging the @ref job%s and locks for thread safety
 *
 */
typedef struct jobQueue {
    /*@{*/
    fifo * jobs; /**< fifo queue containing the jobs */
    pthread_mutex_t lock; /**< mutex lock */
    pthread_cond_t notEmpty; /**< condition variable */
    /*@}*/
} jobQueue;

/**
 * @struct threadpool
 * @brief struct representing a threadpool
 *
 */
typedef struct threadpool {
    /*@{*/
    pthread_t * threads;  /**< the threads of the threadpool */
    int numThreads;  /**< the number of threads */
    jobQueue * queue;  /**< the jobQueue */
    bool isRunning;  /**< if threadpool is active or not */
    /*@}*/
} threadpool;

/////////////
//Functions//
/////////////

void jobDestructor(void* vjob)
{
    free((job*)vjob);
}

jobQueue * jobQueueCreate()
{
    // alloc mem
    jobQueue * queue = malloc(sizeof(jobQueue));

    // create the jobs structure
    queue->jobs = fifo_create(jobDestructor);

    // locks
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->notEmpty, NULL);

    return queue;
}

void jobQueueDestroy(jobQueue * queue)
{
    pthread_mutex_destroy(&(queue->lock));
    pthread_cond_destroy(&(queue->notEmpty));
    fifo_destroy(queue->jobs);
    free(queue);
}

void threadpool_enqueue(threadpool * pool, void(*routine)(void*), void * arg)
{
    job * newJob = malloc(sizeof(job));
    newJob->routine = routine;
    newJob->arg = arg;
    pthread_mutex_lock(&pool->queue->lock);
    if(!pool->isRunning) {
        pthread_mutex_unlock(&pool->queue->lock);
        return;
    }
    if(fifo_isempty(pool->queue->jobs)) {
        pthread_cond_signal(&pool->queue->notEmpty);
    }
    fifo_enqueue(pool->queue->jobs, (void*)newJob);
    pthread_mutex_unlock(&pool->queue->lock);
}

void * doWork(void * voidpool)
{
    threadpool * pool = (threadpool*) voidpool;

    while(1) {
        pthread_mutex_lock(&pool->queue->lock);

        while( pool->isRunning && fifo_isempty(pool->queue->jobs) ) {
            pthread_cond_wait(&pool->queue->notEmpty, &pool->queue->lock);
        }

        if(!pool->isRunning && fifo_isempty(pool->queue->jobs)) {
            break;
        }

        job * j = (job*)fifo_dequeue(pool->queue->jobs);

        pthread_mutex_unlock(&pool->queue->lock);

        if (j != NULL) {
            j->routine(j->arg);
            free(j);
        }
    }
    pthread_mutex_unlock(&pool->queue->lock);
    pthread_exit(NULL);
    return NULL;
}

threadpool * threadpool_create(int numThreads)
{
    // alloc memory
    threadpool * pool = malloc(sizeof(struct threadpool));
    pool->threads = calloc(numThreads, sizeof(pthread_t));
    pool->numThreads = numThreads;

    pool->isRunning = true;

    // create contents
    pool->queue = jobQueueCreate();
    for(int i=0; i<numThreads; ++i) {
        pthread_create(&pool->threads[i], NULL, doWork, pool);
    }


    return pool;
}

void threadpool_destroy(threadpool * pool)
{
    pthread_mutex_lock(&pool->queue->lock);
    pool->isRunning = false;
    pthread_cond_broadcast(&pool->queue->notEmpty);
    pthread_mutex_unlock(&pool->queue->lock);

    for(int i=0; i < pool->numThreads; ++i) {
        if(pthread_join((pool->threads[i]), NULL) != 0) {
            exit(EXIT_FAILURE);
        }
    }
    jobQueueDestroy(pool->queue);
    free(pool->threads);
    free(pool);
}
