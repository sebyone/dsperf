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

#define IPV4_MIN_MSS 256
#define IPV4_MAX_MSS 1460

#define IPV4_MIN_SPORT 0
#define IPV4_MAX_SPORT 65535
#define IPV4_DEF_SPORT 3000



#define _MODEL_DAAS 2

#define MIN_BLOCK_SIZE 1
#define MAX_BLOCK_SIZE _1GB // 1GB




#define _OPT_STR_LEN 256
typedef struct
{
    //bool version;             // ???
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
    size_t block_size;        // block size  (needs to be supported by model)
    int pkt_payload;          // payload size (needs to be supported by model)
    int pkts_num;             // ???
    bool mss_specified;       // ???
                              // ______Time based Test
    bool time_defined;        //
    int time;                 //
                              //
                              // ______Print out options
    bool csv_enabled;         // enables formatter
    char csv_path[_OPT_STR_LEN];       // output filename
    bool csv_format;          // ?
    bool csv_no_header;       // ?
                              //
                              // ______Underlay: IPv4
    int ipv4_port;            // IPv4 Service Port
    char ipv4_addr[_OPT_STR_LEN];      // IPv4 client:remote_addr, server:local-if_addr  
                              //
                              // ______Overlay: DaaS
    int remote_din;           // node to link
    char model_path[_OPT_STR_LEN];     // path to configutation file (libdaas.ini)

} options_t;



void print_usage();

void print_help();

void print_version();

ret_t parse_args(int argc, char *argv[]);

ret_t validate_options();

#endif // OPTIONS_H
