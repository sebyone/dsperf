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

// ipv4_tcp
#define PACKET_BUFFER_MAX_SIZE 2048
//
#define MIN_PORT 1
#define MAX_PORT 65535
//
#define MAX_LINE_LEN 256
#define MAX_LINKS 5
#define MAX_REMOTE_LINKS 64
#define LINK_MAX_VAL 6
typedef enum
{
    _OUTS_CSV_HEADER = 1, // header
    _OUTS_CSV_ROW,        // simple line
    _OUTS_SUMMARY,
    _OUTS_SUMMARY_ROW
} frm_stuffs_e;

#define VARS_COUNTER 13

enum ipv4tcp_vars // Test Model Indicators
{
    _tstcounter = 0, // test executions
    _blocksize,      // "data to send" traffic block size [MB]
    _protocol,       // TEST_MODEL_NAME - Setted
    _pktpayload,     //  packet payload size header not included [bytes]
    _pktheader,      // Header [bytes]   - 17 bytes IP header + sub-protocol options 0..34 bytes
    _efficiency,     // Efficiency [%]   - ratio: [%] = payload / total_packet_size ( header+payload )
    _pktstosend,     // Pkts to send
    _pktssent,       // Pkt sent       - counter of packet really sended ( check socket buffering settings !!!!!!!)
    _pktsloss,       // Pkt loss       - ???????????????????????
    _datasent,       // Data Sent [MB] -
    _pktserror,      // Pkt Err.[%]
    _ttime,          // Transfer Time [ms]
    _throughput      // Throughput [MBps]  [Mbps] [pps]
};

#include "../helpers/utils.h"
#include "../options.h"
#include "../model.h"

// ret_t get_interfaces(lnetif_t *ifs);

ret_t get_env_ipv4tcp(netif_t &_nif); // returns interface information

ret_t set_env_ipv4tcp(netif_t &nif_); // uodates interface parameters

ret_t run_server_ipv4tcp(int nif_, int port_); // start loopback server

ret_t run_client_ipv4tcp(char *server_ip_, int port_); // performs test

#endif // MODEL_IPV4TCP_H
