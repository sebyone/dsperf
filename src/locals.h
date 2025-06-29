/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: locals.h
 *
 * Licence
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind.
 * The entire risk as to the quality and performance of the Covered Software is with You.
 * This disclaimer of warranty constitutes an essential part of this License.
 * No use of any Covered Software is authorized under this License except under this disclaimer.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

#ifndef LOCALS_H
#define LOCALS_H

#pragma once

#include "helpers/utils.h"
// Setting --------------------------------------------------

#undef WITH_DAAS

#undef DEBUG
#define VERBOSE


// ---------------------------------------------- END SETTINGS

#define noexec void(0)

#ifdef VERBOSE
#define pverbose printf
#else
#define pverbose noexec
#endif

#ifdef DEBUG
#define pdebug printf
#else
#define pdebug noexec
#endif

typedef enum
{
    rtOk = 0,   // No error
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
} netif_t;

/*
#if defined(__linux__) || defined(__RASP__) || defined(__MINGW64__)
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#endif


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
*/

#endif // LOCALS_H
