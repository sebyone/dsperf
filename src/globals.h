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

#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once

// -------------------------------------------------Config
#undef LIBDAAS_V19
#undef DEBUG
#define VERBOSE

#include "helpers/utils.h"
#include <stdbool.h>

typedef enum // Supported Protocols
{
  _PROTO_NONE = 0, // Undefined
  _PROTO_DAAS,     // DaaS/Meshin Stack (Overlay)
  _PROTO_IPV4,     // IPv4 Stack (Underlay)
  _PROTO_WLAN,     // Wireless LLC/MAC (Underlay)
  _PROTO_BTHX      // Bluetooth LLC/MAC
} nproto_t;

typedef enum // Roles
{
  _ROLE_NONE = 0, // Undefined
  _ROLE_CLIENT,
  _ROLE_SERVER
} hrole_t;

#define noexec void(0)

#ifdef VERBOSE
#include <stdio.h>
#define pverbose printf
#else
#define pverbose noexec
#endif

#ifdef DEBUG
#include <stdio.h>
#define pdebug printf
#else
#define pdebug noexec
#endif

typedef enum 
{
  rtOk = 0, // No error
  rtExit,   // stdlib EXIT_FAILURE 1
  rtErr     // Generic error
} rt_t;

typedef rt_t (*func_ptr)(); // ret_t  // pointer to tester routine

#define _MAX_STR_LEN 256


// Data Structures for networking local hardware resources
//
typedef struct // interfaces
{
  char *ifname;     // name as OS reported "eth", "wlan", ...
  double bandwidth; // nominal Speed: 50000Mb/s

  /*
$ ethtool eth0
  Options for eth0:
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

#endif // GLOBALS_H
