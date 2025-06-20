/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: model_ipv4tcp.h
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
 * 
 * ToDo:
 * - windows porting
 * - check um and throughput calc
 * - compute bandwidth and throughput % as: throughput / bandwidth * 100
 * - model multi-threading ( socket shared )
 * - model time-windowed
 * 
 */


// Sends a block of data to remote server and print out all test results (test_outs)
//
// Sub Protocol as defined in RFCs 790-791
// 6-TCP	Transmission Control Protocol   

// Numero di protocollo	Nome del protocollo	Abbreviazione
// ----+---------------------------------------+----------
// 1	Internet Control Message Protocol	    ICMP
// 2	Internet Group Management Protocol	    IGMP
// 6	Transmission Control Protocol	        TCP
// 17	User Datagram Protocol	                UDP
// 41	IPv6 encapsulation	                    ENCAP
// 89	Open Shortest Path First	            OSPF
// 132	Stream Control Transmission Protocol	SCTP


#ifndef MODEL_IPV4TCP_H
#define MODEL_IPV4TCP_H

#define TEST_IPV4TCP 1                   // IP sub 6 (TCP)
#define TEST_IPV4TCP_CLASS TEST_CAPACITY // Capabilities, Availability, Capacity, Security

#define TEST_MODEL_NAME "IPv4/TCP"
#define TEST_MODEL_INFO "Throughput/Bandwidth v.01a"
#define TEST_MODEL_LINK "developers@sebyone.it"

#include "locals.h"
#include "options.h"
#include "hardware.h"
#include "utils.h"



// Model Indicators 
enum ipv4tcp_vars
{
    _tstcounter = 0,
    _blocksize, // data to send
    _protocol,
    _pktpayload,
    _pktheader,
    _efficiency,
    _pktstosend,
    _pktssent,
    _pktsloss,
    _datasent,
    _pktserror,
    _ttime,
    _throughput // Mbps
};

// ipv4_tcp
#define MIN_PORT 1
#define MAX_PORT 65535
//
#define MAX_LINE_LEN 256
#define MAX_LINKS 5
#define MAX_REMOTE_LINKS 64
#define LINK_MAX_VAL 6

// ipv4_tcp
#define MIN_PORT 1
#define MAX_PORT 65535

ret_t get_ipv4tcp_info(); // return test model information (string)

ret_t check_ipv4tcp();

ret_t run_ipv4tcp_server(int port); // start loopback server

ret_t run_ipv4tcp_client(const char *server_ip, int server_port); // performs test

#endif // MODEL_IPV4TCP_H
