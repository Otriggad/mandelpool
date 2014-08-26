/**
 * @file fifo.h
 * @author Sebastian Rautila
 * @date 9/5
 * @brief A First In, First Out queue with generalized data payload.
 */

#include "../include/fifo.h"

/**
 * @struct node
 * @brief @ref A node with arbitrary payload and pointer to a next node
 */
typedef struct node {
    /*@{*/
    void* payload; /**< An abitrary payload of the node */
    struct node *next; /**< A pointer to the next node */
    /*@}*/
} node;

/**
 * @struct fifo
 * @brief @ref A First In, First Out queue with generalized data payload.
 */
struct fifo {
    /*@{*/
    node* head; /**< The first @ref node of the @ref fifo*/
    node* last; /**< The last @ref node in the @ref fifo*/
    int length; /**< The length of the @ref fifo */
    void (*payLoadDestructor)(void*); /**< The destroy function for the payload */
    /*@}*/
};

fifo* fifo_create(void (*payLoadDestructor)(void*))
{
    fifo* newFifo = malloc(sizeof(fifo));
    newFifo->head = NULL;
    newFifo->last = NULL;
    newFifo->length = 0;
    newFifo->payLoadDestructor = payLoadDestructor;
    return newFifo;
}

void fifo_destroy(fifo* q)
{
    while (q->last != NULL) {
        node* removeNode = q->last;
        q->last = q->last->next;
        q->payLoadDestructor(removeNode->payload);
        free(removeNode);
    }
    free(q);
}

bool fifo_isempty(fifo* q)
{
    if (q->length == 0) {
        return true;
    } else	{
        return false;
    }
}

void fifo_enqueue(fifo* q, void* payload)
{
    node* newNode = malloc(sizeof(node));
    newNode->payload = payload;
    newNode->next = NULL;

    if (q->head != NULL) {
        q->head->next = newNode;
    } else {
        q->last = newNode;
    }
    q->head = newNode;
    ++q->length;
}

void* fifo_dequeue(fifo* q)
{
    if (q->last == NULL) {
        return NULL;
    }
    node* toRemove = q->last;
    q->last = toRemove->next;
    void* payload = toRemove->payload;
    free(toRemove);
    --q->length;
    if(q->length == 0) {
        q->head = NULL;
    }
    return(payload);
}
