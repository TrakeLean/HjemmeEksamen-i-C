
#include <stdio.h>
#include <sys/time.h>
#include "common.h"

unsigned long long gettime()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1)
    {
        fprintf(stderr, "Could not get time\n");
        return -1;
    }

    unsigned long long micros = 1000000 * tv.tv_sec + tv.tv_usec;

    return micros;
}
