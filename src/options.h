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
#include "models.h"

#define _MAX_OPT_LEN _MAX_STR_LEN

typedef struct
{
    nproto_t model_protocol;               // 0 = unset, _PROTO_DAAS,  _PROTO_IPV4, ...
    tclass_t model_class;                  // 0 = unset, Features, Capacity, Continuity, etc.etc.
                                           //
    hrole_t run_mode;                      // 0 = unset, _ROLE_CLIENT, _ROLE_SERVER, ...
    int tst_repeats;                       // repeats test, default is 1
    long tst_time_slot;                    // perform testing for a fixed time period, in seconds
    long long tst_block_size;              // generate traffic by block size
    long tst_block_size_str[_MAX_OPT_LEN]; // generate traffic by block size
    long tst_pkts_num;                     // generate traffic by number of pkts (requires pkt_payload)
                                           //
                                           //  _____Tester parmaters
    int pkt_payload;                       // pkt's payload size (needs to be supported in testing routine)
    int multilink;                         // enable to use concurrent connections
                                           //
                                           // ______Capacity
    long bandwidth;                        // set reference to nominal speed [Mbps]
                                           //
                                           // ______Peers addressing
    char local_hwif_str[_MAX_OPT_LEN];     // Protocol Local-IF/Addr
    char local_addr[_MAX_OPT_LEN];         // Protocol Local-IF/Addr
    char remote_addr[_MAX_OPT_LEN];        // Protocol Remote Addr (URI Loopback Service)
    char service_str[_MAX_OPT_LEN];        // Service Port Name
    int service_num;                       // Service Port

    //
    // ______Output formatting
    bool csv_enabled;            // output will be formatted
    bool csv_format;             //
    bool csv_no_header;          // csv columns header is/not printed out
    char csv_path[_MAX_OPT_LEN]; // output will be saved
                                 // char model_path[_OPT_STR_LEN];  // path to configutation file (libdaas.ini)

} options_t;

#define MIN_BLOCK_SIZE 1
#define MIN_PKT_NUMBER 1

#pragma once

void print_credits();
void print_usage();
void print_help();

rt_t parser_args(int argc, char *argv[]);

#endif // OPTIONS_H
