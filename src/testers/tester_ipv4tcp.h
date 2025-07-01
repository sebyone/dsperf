/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: tester_ipv4tcp.h
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

 
/* dsperf tester routine
   ------------------------------------------
          title: "IPv4/TCP"
          specs: "Throughput/Bandwidth v.01a"
         author: "developers@sebyone.it"
       protocol: _PROTO_IPV4,
          model: __Capacity
        variant: 0

    Overview
    Trasmission Capacity Tester routine for Transmission Control Protocol (6-TCP)
    Sends a block of data to remote server and print out all test results.

    Protocol and subs as defined in IETF RFCs 790-791
    ----+---------------------------------------+----------
    1	Internet Control Message Protocol	    ICMP
    2	Internet Group Management Protocol	    IGMP
    6	Transmission Control Protocol	        TCP
    17	User Datagram Protocol	                UDP
    41	IPv6 encapsulation	                    ENCAP
    89	Open Shortest Path First	            OSPF
    132	Stream Control Transmission Protocol	SCTP
*/

#ifndef MODEL_IPV4TCP_H
#define MODEL_IPV4TCP_H

#include "../models/capacity.h"
#include "../options.h"

ret_t set_env_ipv4tcp(options_t &ops_); // set default Options values for model

ret_t run_server_ipv4tcp(); // start loopback server

ret_t run_client_ipv4tcp(); // performs test

#endif // MODEL_IPV4TCP_H
