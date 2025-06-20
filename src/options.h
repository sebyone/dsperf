
/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: block_runner.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind.
 * The entire risk as to the quality and performance of the Covered Software is with You.
 * This disclaimer of warranty constitutes an essential part of this License.
 * No use of any Covered Software is authorized under this License except under this disclaimer.
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <stdbool.h>

#include "locals.h"

#include "models\model_ipv4tcp.h"
#include "models\model_daasfrs.h"

#pragma once

// cli args _____________________________________________________________

typedef struct
{
    int model;       // -1 = unset,
                     // 3:0 = ipv4_tcp (method flow&time for throughput test)
                     // 3:1 = daas
                     // 3:2 = ipv4_tcp - (method file/block-size total transfer time )
                     //
    int host_role;   // -1 = unset, 0 = server, 1 = client 2 = router
    int repetitions; // solo client, default 1
    ///
    bool csv_enabled;   // enables formatter
    char csv_path[256]; // output filename
    bool csv_format;    // ?
    bool csv_no_header; // ?
    ///
    // int model_sub // -1 unset, daas set fix driver: 1 inet4udp, 2 inet4tcp
    long int block_size; // traffic block size  (needs to be supported by model)
    int pkt_payload;     // payload size (needs to be supported by model)

    bool mtu_specified; // ???
    int pack_num;     // ???

    int port;              // server con underlay: porta di ascolto
    char remote_addr[256]; // client con underlay: IP:PORT stringa
    int remote_din;        // client/server con daas: remote DIN (intero)

    char model_path[256]; // percorso file ini per daas

    int time;
    bool time_defined;
    bool version;
} options_t;

void print_usage(const char *prog_name);

void print_options(const char *prog_name);

ret_t parse_args(int argc, char *argv[]);

ret_t validate_args(const char *prog_name);

#endif // OPTIONS_H