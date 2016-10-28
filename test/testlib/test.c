#include <stdio.h>
#include <stdlib.h>

void test_function(void)
{
    fprintf(stderr, "hallo from android test.so\n");
    fprintf(stderr, "testing a floating point argument: %f\n", 3.14);
    fprintf(stderr, "testing again\n");
}
