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

#ifndef SYSTEM_H
#define SYSTEM_H

#pragma once

#include "globals.h"

#include <stdlib.h>

typedef struct
{
  int number;
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
} hwif_t, *phwif_t;

rt_t ipv4tcp_bandwidth(int ifn, long dband);

#endif // SYSTEM_H
