#include "datetime.h"

#include <time.h>
#include <sys/time.h>

static struct timeval __tv;
static struct timespec __ts;

double now_in_seconds()
{
    gettimeofday(&__tv, NULL);  // (https://man7.org/linux/man-pages/man2/settimeofday.2.html)
    return __tv.tv_sec + (__tv.tv_usec / 1000000.0);
}


double now_sec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}


    /*
    double now_in_seconds()
    {
        gettimeofday(&__tv, NULL);
        return __tv.tv_sec + (__tv.tv_usec / 1000000.0);
    }

    double get_time_microseconds()
    {
        clock_gettime(CLOCK_MONOTONIC, &__ts);
        return __ts.tv_sec * 1e6 + __ts.tv_nsec / 1000;
    }


    double now_sec()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1e6;
    }

    double now_in_seconds()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + (tv.tv_usec / 1000000.0);
    }

    static double get_time_microseconds()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1e6 + ts.tv_nsec / 1000;
    }
    */
