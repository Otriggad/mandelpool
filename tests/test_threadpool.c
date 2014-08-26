/**
 * @file test_threadpool.c
 * @author Sebastian Rautila, Andreas Widmark
 * @date 13/5 2014
 * @brief Test for the threadpool using fib
 */

#include "minunit.h"
#include "../src/threadpool.h"

int fib(int a)
{
    if(a == 1 || a == 0) return 1;
    else return fib(a - 1) + fib(a - 2);
}

void job(void * arg)
{
    int * iarg = (int*) arg;
    for(int a = 1; a < *iarg; a++) {
        printf("%i: %i\n", a, fib(a));
    }
    printf("Job: %i done\n", *iarg);
}

void test_setup()
{

}

void test_teardown()
{
    // Nothing
}

MU_TEST(test_dummy)
{
    mu_assert(true == true, "dummy");
}

MU_TEST_SUITE(test_suite)
{
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    MU_RUN_TEST(test_dummy);

}

int main(int argc, char *argv[])
{
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}
