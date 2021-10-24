#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

void main (void)
{
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    int inicio = (int)(begin.tv_nsec / 1.0e6);


    clock_gettime(CLOCK_REALTIME, &end);

    int fin = (int)(begin.tv_nsec / 1.0e6);

    int seconds = fin - inicio;
}