/*
 * test_fifo
 *
 *  Created on: May 9, 2014
 *      Author: AW
 */

#include "minunit.h"
#include "../src/fifo.h"

void intDestruct(void * payload)
{
    free((int*)payload);
}

void test_setup()
{

}

void test_teardown()
{
    // Nothing
}

//test fifo_isempty no insertion
MU_TEST(test_fifo_isempty)
{
    fifo* testFifo1 = fifo_create(intDestruct);
    mu_assert((fifo_isempty(testFifo1) == true), "testFifo1 should be empty (true)");
}
//test fifo_isempty non empty queue after enqueue
MU_TEST(test_fifo_isemptyAfterEnqueue)
{
    fifo* testFifo2 = fifo_create(intDestruct);
    int a = 5;
    int b = 6;
    fifo_enqueue(testFifo2, &a);
    fifo_enqueue(testFifo2, &b);
    mu_assert((fifo_isempty(testFifo2) == false), "testFifo2 should not be empty (false)");
}

//test fifo_isempty after dequeue
MU_TEST(test_fifo_isemptyAfterDequeue)
{
    fifo* testFifo3 = fifo_create(intDestruct);
    int val1 = 1;
    int val2 = 2;
    int val3 = 3;
    int val4 = 4;

    fifo_enqueue(testFifo3, &val1);
    fifo_enqueue(testFifo3, &val2);
    fifo_enqueue(testFifo3, &val3);
    fifo_enqueue(testFifo3, &val4);

    fifo_dequeue(testFifo3);
    fifo_dequeue(testFifo3);
    fifo_dequeue(testFifo3);
    fifo_dequeue(testFifo3);
    mu_assert((fifo_isempty(testFifo3) == true), "testFifo3 should be empty (true)");
}

MU_TEST(test_fifo_enqueue)
{
    fifo* testFifo4 = fifo_create(intDestruct);
    int val1 = 1;
    int val2 = 2;
    int val3 = 3;
    int val4 = 4;

    fifo_enqueue(testFifo4, &val1);
    fifo_enqueue(testFifo4, &val2);
    fifo_enqueue(testFifo4, &val3);
    fifo_enqueue(testFifo4, &val4);

    mu_assert(*(int*)(fifo_dequeue(testFifo4)) == 1, "dequeue should return 1");
}

MU_TEST(test_fifo_dequeue)
{
    fifo* testFifo = fifo_create(intDestruct);

    mu_assert(fifo_dequeue(testFifo) == NULL, "dequeing an empty q should return NULL 0");
    int val4 = 1;
    fifo_enqueue(testFifo, &val4);
    mu_assert(*(int*)(fifo_dequeue(testFifo)) == 1, "dequeing should return 1");
    mu_assert(fifo_dequeue(testFifo) == NULL, "dequeing an empty q should return NULL 1");
    mu_assert(fifo_dequeue(testFifo) == NULL, "dequeing an empty q should return NULL 2");
}

MU_TEST_SUITE(test_suite)
{
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(test_fifo_isempty);
    MU_RUN_TEST(test_fifo_isemptyAfterEnqueue);
    MU_RUN_TEST(test_fifo_isemptyAfterDequeue);
    MU_RUN_TEST(test_fifo_enqueue);
    MU_RUN_TEST(test_fifo_dequeue);
}

int main(int argc, char *argv[])
{
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}

