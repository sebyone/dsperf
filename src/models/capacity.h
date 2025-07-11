/*
 * dsperf 2024, 2025 (@) Sebyone Srl
 *
 * File: capacity.h
 *
 * Licence
 * This Source Code Form  is subject to the terms of the Mozilla Public License, v. 2.0.
 * You can obtain a copy of the MPL License at https://mozilla.org/MPL/2.0/.
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty
 * of any kind.The entire risk as to the quality and performance of the Covered Software
 * is  with You.  This  disclaimer  of  warranty  constitutes  an essential part of this
 * License. No use of any Covered Software is authorized under this License except under
 * this disclaimer.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 *
 */

/*
Latency refers to the delay in data transmission, while .
*/
#ifndef MODEL_CAPACITY_H
#define MODEL_CAPACITY_H

#pragma once
#include <stdlib.h>
// #include <vector>

#include "../models.h"

// ------------------------------------------------------------------------
// MODEL VARS CAPACITY
// ------------------------------------------------------------------------

typedef struct // Model vars Capacity test
{
    int tstcounter;       //  1. Executions counter
                          //  Testing mode:
    double blocksize;     //  2. Data block size used to generate Traffic (datatosend) [MB]
    double timeslot;      //  1. Time period will be used to test how many data can be transferred [s]
    double pktstosend;    //  7. Number of Pkts will be sent, computed or user setted [n]
                          //
    double pktheader;     //  5. Header [bytes]
    double pktpayload;    //  4. Payload size [bytes]
    double pktefficiency; //  6. Protocol Efficiency [%] - ratio: [%] = payload / packet_size ( header+payload )
    char if_name[32];     //  6. Protocol Efficiency [%] - ratio: [%] = payload / packet_size ( header+payload )
    double if_mtu;        //  6. Protocol Efficiency [%] - ratio: [%] = payload / packet_size ( header+payload )
    double fragmentation; //  6. Packets fragmentation
                          //
    double datasent;      // 10. Traffic data sent [MB]
    double datarcvd;      //  8. Pkt received
    double dataloss;      //  9. Traffic loss  = pktssent - pktstosend
                          //
    double pktssent;      //  8. Pkt sent       - counter of packet really sended ( check socket buffering settings !!!!!!!)
    double pktsrcvd;      //  8. Pkt received
    double pktsloss;      //  9. Pkt loss  = pktssent - pktstosend     - ???????????????????????
                          //
    double opentime;      // 12. Total links management time  [ms]
    double conntime;      // 12. Total elapsed test time [ms]
    double rtriptime;     // Time to send-receive_ack
                          //
    double throughput;    // 13. Throughput [Mbps]
    double bandwidth;     // set reference to nominal speed [Mbps]
    double saturation;    // Bandwidth saturation %
    double latyency;      // Latency refers to the delay in data transmission
    double jitter;        // jitter is the variation in that delay

    //

    //
    double tsterrors; // 11.
                      //
} capacity_vars_t;

// ------------------------------------------------------------------------------------------------------------------------------!
rt_t resetVars(capacity_vars_t &_cvars);
rt_t report_capacity(const model_info_t *info_, capacity_vars_t *vars_, report_item_t switch_);

#endif // MODEL_CAPACITY_H
