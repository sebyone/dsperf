/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: loopback.c
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 * m.pagano@sebyone.it
 * 
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Timer struct
    typedef struct
    {
        unsigned int interval_us;
        pthread_t thread;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        bool running;
        bool tick;
    } dsperf_timer_t;

    // Create a timer that "wakes up" in an defined interval
    dsperf_timer_t *dsperf_timer_create(unsigned int interval_us);

    // Destroy timer and resource
    void dsperf_timer_destroy(dsperf_timer_t *t);

    // Start timer in an internal thread
    void dsperf_timer_start(dsperf_timer_t *t);

    // Stop timer (wait for the internal thread)
    void dsperf_timer_stop(dsperf_timer_t *t);

    // the client calls this function to wait for the time
    // returns true if timer is active, false otherwise
    bool dsperf_timer_wait_tick(dsperf_timer_t *t);

#ifdef __cplusplus
}
#endif

#endif
