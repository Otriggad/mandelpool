/**
 * @file fifo.h
 * @author Sebastian Rautila
 * @date 9/5
 * @brief A First In, First Out queue with generalized data payload.
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * @struct fifo
 * @brief @ref A First In, First Out queue with generalized data payload.
 */
typedef struct fifo fifo;

/**
 * @brief Creates a FIFO-queue.
 * @param payloadDestructor Function to free the data stored in the FIFO. Should take one argument of type void* (the payload).
 * @return A pointer to a fresh FIFO.
 */
fifo* fifo_create(void (*payloadDestructor)(void*));

/**
 * @brief Destroys a FIFO-queue and frees all resources (if payloadDestructor is defined correctly).
 * @param q The queue to destroy.
 */
void fifo_destroy(fifo* q);

/**
 * @brief If queue is empty.
 * @param q The FIFO-queue
 * @return Whether the queue is empty or not.
 */
bool fifo_isempty(fifo* q);

/**
 * @brief Enqueues a payload to the FIFO-queue.
 * @param q The queue.
 * @param payload The data to add to the queue.
 */
void fifo_enqueue(fifo* q, void* payload);

/**
 * @brief Dequeues an element from the FIFO.
 * @param q The queue.
 * @return The payload which was added least recently to the queue.
 */
void* fifo_dequeue(fifo* q);



#endif /* FIFO_H_ */
