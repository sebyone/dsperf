/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: block_runner.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind, either
 * expressed, implied, or statutory, including, without limitation, warranties that the Covered  Software is
 * free of defects, merchantable, fit for a particular purpose or non-infringing.
 * The entire risk as to the quality and performance of the Covered Software is with You.  Should any Covered
 * Software prove defective in any respect, You (not any Contributor) assume the cost of any necessary
 * servicing, repair, or correction.
 * This disclaimer of warranty constitutes an essential part of this License.  No use of any Covered Software
 * is authorized under this License except under this disclaimer.
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

// Sub Protocol as defined in RFCs 790-791
//
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

#define TEST_MODEL_NAME "IPv4/TCP" // IP sub 6 (TCP)
#define TEST_MODEL_TYPE 4          // [1..4] Capabilities, Availability, Capacity, Security
#define TEST_MODEL_INFO "Throughput/Bandwidth v.01a"
#define TEST_MODEL_LINK "developers@sebyone.it"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(__linux__) || defined(__RASP__)
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif

// #include <time.h>
// #include "../timer.h"


#include "helpers.h"
#include "locals.h"
#include "options.h"

// ipv4_tcp
#define MIN_PORT 1
#define MAX_PORT 65535

ret_t ipv4tcp_info(); // return test model information (string)

ret_t run_ipv4tcp_server(int port); // start loopback server

ret_t run_ipv4tcp_client(options_t *test, const char *server_ip, int server_port); // performs test

#endif // MODEL_IPV4TCP_H
