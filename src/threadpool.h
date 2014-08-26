/**
 * @file threadpool.h
 * @author Sebastian Rautila, Andreas Widmark
 * @date 7/5 2014
 * @brief Contains the interface for the threadpool
 */

#ifndef THREADPOOL__H
#define THREADPOOL__H
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "fifo.h"

/**
 * @struct threadpool
 * @brief the @ref threadpool struct created with pool_create
 */
struct threadpool;

/**
 * @brief Creates a threadpool.
 * @param numThreads Designates the fixed amount of threads.
 * @return The created threadpool.
 */
struct threadpool * threadpool_create(int numThreads);

/**
 * @brief Destroys the designated threadpool.
 * @param pool The pool to destroy.
 * //param force If false the threadpool waits until the job queue is empty (while accepting no new jobs).
 */
void threadpool_destroy(struct threadpool * pool);

/**
 * @brief Adds a job to the designated threadpool.
 * @param pool Threadpool to add job to.
 * @param routine Function to be run. Must be a function which takes one argument.
 * @param arg The argument to routine.
 *
 */
void threadpool_enqueue(struct threadpool * pool, void (*routine)(void*), void * arg);


#endif // THREADPOOL__H
