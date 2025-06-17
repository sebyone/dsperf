
/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: block_runner.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind, either
 * expressed, implied, or statutory, including, without limitation, warranties that the Covered  Software is
 * free of defects, merchantable, fit for a particular purpose or non-infringing.
 * The entire risk as to the quality and performance of the Covered Software is with You.  Should any Covered
 * Software prove defective in any respect, You (not any Contributor) assume the cost of any necessary
 * servicing, repair, or correction.
 * This disclaimer of warranty constitutes an essential part of this License.  No use of any Covered Software
 * is authorized under this License except under this disclaimer.
 *
 * Limitation of Liability
 * Under no circumstances and under no legal theory, whether tort (including negligence), contract, or otherwise,
 * shall any Contributor, or anyone who distributes Covered Software as permitted above, be liable to You for
 * any direct, indirect, special, incidental, or consequential damages of any character including, without
 * limitation, damages for lost profits, loss of goodwill, work stoppage, computer failure or malfunction,
 * or any and all other commercial damages or losses, even if such party shall have been informed of the
 * possibility of such damages.  This limitation of liability shall not apply to liability for death or personal
 * injury resulting from such party's negligence to the extent applicable law prohibits such limitation.
 * Some jurisdictions do not allow the exclusion or limitation of incidental or consequential damages, so this
 * exclusion and limitation may not apply to You.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

#ifndef DSPERF_LOCALS_H
#define DSPERF_LOCALS_H

#pragma once

#if defined(__linux__) || defined(__RASP__)
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#endif

#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

// Configure
#define WITH_DAAS

#define MAX_LINE_LEN 256
#define MAX_LINKS 5
#define MAX_REMOTE_LINKS 64
#define LINK_MAX_VAL 6

//
#define MIN_BLOCK_SIZE 1
#define MAX_BLOCK_SIZE (1024 * 1024 * 1024) // 1GB
#define MIN_PACKET_SIZE 1
#define MAX_PACKET_SIZE (10 * 1024 * 1024) // 10MB


typedef enum : unsigned short // Functions return code
{
    rtOK = 0,   // No error
    rtExit = 1, // stdlib EXIT_FAILURE 1
    rtErr = 2   // Generic error
} ret_t;

// Data Structures for local hardware resources
//
typedef struct // local interfaces
{
    char *ifname;     // name as OS reported "eth", "wlan", ...
    double bandwidth; // nominal Speed: 50000Mb/s

    /*
  $ ethtool eth0
    Settings for eth0:
    Supported ports: [ FIBRE ]
    Supported link modes:   25000baseCR/Full, 50000baseCR2/Full
    Supported pause frame use: Symmetric Receive-only
    Supports auto-negotiation: Yes
    Supported FEC modes: RS	 BASER	 LLRS
    Advertised link modes:  25000baseCR/Full
                            50000baseCR2/Full
    Advertised pause frame use: Symmetric
    Advertised auto-negotiation: Yes
    Advertised FEC modes: Not reported
    Link partner advertised link modes:  Not reported
    Link partner advertised pause frame use: Symmetric
    Link partner advertised auto-negotiation: No
    Link partner advertised FEC modes: Not reported
    Speed: 50000Mb/s
    Lanes: 2
    Duplex: Full
    Auto-negotiation: on
    Port: FIBRE
    PHYAD: 0
    Transceiver: internal
    netlink error: Operation not permitted
    Current message level: 0x00002081 (8321)
                           drv tx_err hw
    Link detected: yes
    */
} localif_t;

ret_t get_interfaces(localif_t &ifs); // Returns a list of availaible local interfaces ( )




// -------------------------------------------------------------------------------------------------------- !
static struct timeval __tv;
static struct timespec __ts;

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

/*
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

#endif // DSPERF_LOCALS_H