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

#include "globals.h"

#define MIN_BLOCK_SIZE 1
#define MIN_PKT_NUMBER 1

// #define MAX_BLOCK_SIZE _1GB // 1GB

#define _OPT_STR_LEN 256
typedef struct
{
    nproto_t model_protocol;  // 0 = unset, _PROTO_DAAS,  _PROTO_IPV4, ...
    tclass_t model_class;     // 0 = unset, Features, Capacity, Continuity, etc.etc.
    hrole_t host_role;        // 0 = unset, _ROLE_CLIENT, _ROLE_SERVER, ...
                              //
    int repetitions;          // repeats test, default 1
                              //  __Capacity:
    unsigned long block_size; // generate traffic by block size  (needs to be supported in testing routine)
    unsigned long pkts_num;   // generate traffic by number of pkts (needs to be supported in testing routine)
    unsigned pkt_payload;     // pkt's payload size (needs to be supported in testing routine)
                              //
    double testing_time;      // perform testing for a fixed time period, in seconds
                              //
    // ______Print out options
    bool csv_enabled;               // output will be formatted
    bool csv_format;                // ?
    bool csv_no_header;             // ?
    char csv_path[_OPT_STR_LEN];    // output will be saved
                                    //
    long unsigned service_num;      // Protocol Loopback Service Port
    char local_addr[_OPT_STR_LEN];  // Protocol Local-IF/Addr
    char remote_addr[_OPT_STR_LEN]; // Protocol Remote Addr (URI Loopback Service)
                                    // char model_path[_OPT_STR_LEN];  // path to configutation file (libdaas.ini)
} options_t;

#pragma once

void print_credits();

ret_t parse_args(int argc, char *argv[]);

ret_t validate_model_options(exefunc_t &_pfrun);

#endif // OPTIONS_H
