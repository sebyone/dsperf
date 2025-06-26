/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: options.h
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "locals.h"

#pragma once

//
#define _ROLE_SERVER 0
#define _ROLE_CLIENT 1
//
#define _MODEL_IPV4 1
#define _MODEL_DAAS 2

typedef struct
{
    bool version;             // ???
                              // ______Testing routine
    int model;                // -1 = unset,
                              // 3:0 = ipv4tcp (method flow&time for throughput test)
                              // 3:1 = daas
                              // 3:2 = ipv4tcp - (method file/block-size total transfer time )
    int n_lifn;               // local interface !!!!!!!
    int host_role;            // -1 = unset, 0 = server, 1 = client 2 = router
    int repetitions;          // repeats test, default 1
                              //
                              // ______Pkts based test
    unsigned long block_size; // block size  (needs to be supported by model)
    int pkt_payload;          // payload size (needs to be supported by model)
    int pkts_num;             // ???
    bool mss_specified;       // ???
                              // ______Time based Test
    bool time_defined;        //
    int time;                 //
                              //
                              // ______Print out options
    bool csv_enabled;         // enables formatter
    char csv_path[256];       // output filename
    bool csv_format;          // ?
    bool csv_no_header;       // ?
                              //
                              // ______Underlay: IPv4
    int port;                 // server con underlay: porta di ascolto
    char remote_addr[256];    // client con underlay: IP:PORT stringa
    int remote_din;           // client/server con daas: remote DIN (intero)
                              //
                              // ______Overlay: DaaS
    char model_path[256];     // percorso file ini per daas

} options_t;

void print_usage();

void print_help();

void print_version();

ret_t parse_args(int argc, char *argv[]);

ret_t validate_args(const char *prog_name);

#endif // OPTIONS_H
